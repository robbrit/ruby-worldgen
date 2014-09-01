#include <ruby.h>
#include <stdlib.h>

#include "common.h"

#define DEFAULT_ROUGHNESS 5

extern heightmap get_heights(VALUE);
extern void set_heights(VALUE, heightmap_points);
extern int get_size(VALUE);

double diamond_shift(double roughness) {
  double r = (double)rand() / (double)RAND_MAX;

  return (r * 2.0 - 1.0) * roughness;
}

/**
 * Generate terrain using a diamond square algorithm.
 * Arguments:
 * `heightmap` - The heightmap to use for the algorithm
 * `roughness` (optional) - How "rough" to make the surface
 *
 * Return value: nil
 **/
VALUE diamond_square(int argc, VALUE *argv, VALUE self) {
  int x, y;
  double ratio = 500.0;
  VALUE heightmap_obj, vroughness;
  int size, side_size;
  double roughness;
  heightmap_points heights;

  rb_scan_args(argc, argv, "11", &heightmap_obj, &vroughness);
    
  size = get_size(heightmap_obj);
  side_size = size - 1;

  roughness = vroughness == Qnil ? DEFAULT_ROUGHNESS : NUM2DBL(vroughness);

  heights = (heightmap_points)malloc(sizeof(double) * num_points(size));
  memset(heights, 0.0, sizeof(double) * num_points(size));

  ARR(heights, 0, 0) = ARR(heights, 0, size - 1) =
    ARR(heights, size - 1, 0) = ARR(heights, size - 1, size - 1) =
    diamond_shift(roughness);

  while (side_size >= 2) {
    int half_side = side_size / 2;

    // Square step
    for (x = 0; x < size - 1; x += side_size) {
      for (y = 0; y < size - 1; y += side_size) {
        double avg = (ARR(heights, x, y) +
          ARR(heights, x + side_size, y) +
          ARR(heights, x, y + side_size) +
          ARR(heights, x + side_size, y + side_size)) / 4.0;

        ARR(heights, x + half_side, y + half_side) = avg + diamond_shift(roughness);
      }
    }

    for (x = 0; x < size - 1; x += half_side) {
      for (y = (x + half_side) % side_size; y < size - 1; y += side_size) {
        double avg = (ARR(heights, (x - half_side + size - 1) % (size - 1), y) +
          ARR(heights, (x + half_side) % (size - 1), y) +
          ARR(heights, x, (y + half_side) % (size - 1)) +
          ARR(heights, x, (y - half_side + size - 1) % (size - 1))) / 4.0;

        avg += diamond_shift(roughness);

        ARR(heights, x, y) = avg;

        if (x == 0) {
          ARR(heights, size - 1, y) = avg;
        }
        if (y == 0) {
          ARR(heights, x, size - 1) = avg;
        }
      }
    }

    side_size /= 2.0;
    ratio /= 2.0;
  }

  // normalize
  normalize(heights, size);

  // copy into the array
  set_heights(heightmap_obj, heights);

  return Qnil;
}

void load_diamond_square() {
  VALUE mod, algos;

  mod = rb_define_module("Worldgen");
  algos = rb_define_module_under(mod, "Algorithms");

  rb_define_singleton_method(algos, "diamond_square!", diamond_square, -1);
}
