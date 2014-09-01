#include <ruby.h>

#include "common.h"
#include "randomlattice.h"

extern int get_size(VALUE heightmap);
extern void set_heights(VALUE heightmap_obj, heightmap_points map);

/**
 * Generate Perlin-noise
 * Arguments:
 * `heightmap` - The heightmap to use for the algorithm
 * `octaves` (optional, default 4) - How "rough" to make the surface
 * `coefficient` (optional, default 2) - Fractal coefficient
 * `lacunarity` (optional, default 2) - Scaling factor
 *
 * Return value: nil
 **/
VALUE fbm(int argc, VALUE *argv, VALUE self) {
  VALUE heightmap_obj, voctaves, vcoefficient, vlacunarity;
  int octaves, i;
  double lattice_size, lattice_scale = 0.5;
  int x, y, size;
  double fx, fy;
  double total, coefficient, lacunarity, amplitude, frequency;
  heightmap_points heights;
  RandomLattice lattice;

  rb_scan_args(argc, argv, "13", &heightmap_obj, &voctaves, &vcoefficient, &vlacunarity);

  octaves = voctaves == Qnil ? 4 : FIX2INT(voctaves);
  coefficient = vcoefficient == Qnil ? 2.0 : FIX2INT(vcoefficient);
  lacunarity = vlacunarity == Qnil ? 2.0 : FIX2INT(vlacunarity);
    
  size = get_size(heightmap_obj);
  lattice_size = size * lattice_scale;
  create_lattice(&lattice, lattice_size, lattice_size, -1);
  
  heights = (heightmap_points)malloc(sizeof(double) * num_points(size));
  memset(heights, 0.0, sizeof(double) * num_points(size));

  // do the fbm dance
  // TODO: this stuff could be sped up bigtime using threads or GPU
  for (x = 0; x < size; x++) {
    for (y = 0; y < size; y++) {
      fx = x * lattice_scale;
      fy = y * lattice_scale;
      total = 0.0;
      amplitude = 1.0;
      frequency = 1.0;

      for (i = 0; i < octaves; i++) {
        total += value_at(&lattice, fx * frequency, fy * frequency) * amplitude;
        frequency *= coefficient;
        amplitude /= lacunarity;
      }

      ARR(heights, x, y) = total;
    }
  }

  normalize(heights, size);

  // copy into the array
  set_heights(heightmap_obj, heights);

  return Qnil;
}

void load_fbm() {
  VALUE mod, algos;

  mod = rb_define_module("Worldgen");
  algos = rb_define_module_under(mod, "Algorithms");

  rb_define_singleton_method(algos, "fbm!", fbm, -1);
}
