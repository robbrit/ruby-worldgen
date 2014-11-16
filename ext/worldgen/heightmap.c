#include <ruby.h>

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

VALUE HeightmapData; // conversion between heightmap struct and Ruby

void delete_heightmap(void * ptr) {
  heightmap *map = (heightmap *)ptr;
  free(map->heights);
}

/**
 * Get the heightmap for a HeightMap object
 */
heightmap get_heights(VALUE heightmap_obj) {
  VALUE heights_ptr = rb_iv_get(heightmap_obj, "@heights_ptr");
  heightmap *map;

  Data_Get_Struct(heights_ptr, heightmap, map);

  return *map;
}

int get_size(VALUE heightmap_obj) {
  return FIX2INT(rb_iv_get(heightmap_obj, "@size"));
}

void set_heights(VALUE heightmap_obj, heightmap_points map) {
  // if we already have a height, kill it
  VALUE heights_ptr;
  heightmap * hmap;

  hmap = ALLOC(heightmap);
  hmap->heights = map;
  hmap->size = get_size(heightmap_obj);

  heights_ptr = Data_Wrap_Struct(HeightmapData, 0, delete_heightmap, hmap);

  rb_iv_set(heightmap_obj, "@heights_ptr", heights_ptr);
}

/**
 * Initialize the C side of things for a heightmap
 */
VALUE initialize_native(VALUE self, VALUE vsize) {
  int size = FIX2INT(vsize);
  int memsize = num_points(size) * sizeof(double);

  heightmap_points map = (heightmap_points)malloc(memsize);
  memset(map, 0, memsize);
  set_heights(self, map);

  return Qnil;
}

/**
 * Get the number of points within the heightmap. Right now this is a very
 * simple calculation of size * size.
 */
VALUE num_points_wrapped(VALUE self) {
  return INT2FIX(num_points(FIX2INT(rb_iv_get(self, "@size"))));
}

/**
 * Iterate over all the points in the heightmap.
 * Example:
 * ```
 * heightmap.each_height do |x, y, height|
 *   puts "Height at (#{x}, #{y}) is #{height}"
 * end
 */
VALUE each_height(VALUE self) {
  heightmap map = get_heights(self);
  heightmap_points ptr = map.heights;
  VALUE args = rb_ary_new2(3);
  int size = get_size(self);
  int x, y;

  for (x = 0; x < size; x++) {
    rb_ary_store(args, 0, INT2FIX(x));
    for (y = 0; y < size; y++) {
      rb_ary_store(args, 1, INT2FIX(y));
      rb_ary_store(args, 2, DBL2NUM(*ptr++));
      rb_yield(args);
    }
  }
  return self;
}

/**
 * Get the value of our heightmap at a specified point.
 */
VALUE get_at(VALUE self, VALUE vx, VALUE vy) {
  int x = FIX2INT(vx);
  int y = FIX2INT(vy);
  heightmap map = get_heights(self);
  int size = map.size;

  return ARR(map.heights, x, y);
}

/**
 * Set the value of our heightmap at a specified point.
 */
VALUE set_at(VALUE self, VALUE vx, VALUE vy, VALUE vheight) {
  int x = FIX2INT(vx);
  int y = FIX2INT(vy);
  double height = NUM2DBL(vheight);
  heightmap map = get_heights(self);
  int size = map.size;

  ARR(map.heights, x, y) = height;

  return vheight;
}

/**
 * Normalize the heightmap to be between a specified range. The default range
 * is from 0 to 1.
 * Arguments:
 * * min - Minimum value for normalization (default 0).
 * * max - Maximum value for normalization (default 1).
 */
VALUE normalize_hm(int argc, VALUE *argv, VALUE self) {
  heightmap map = get_heights(self);
  VALUE vmin, vmax;
  double min, max;

  rb_scan_args(argc, argv, "02", &vmin, &vmax);

  min = vmin == Qnil ? 0.0 : NUM2DBL(vmin);
  max = vmax == Qnil ? 1.0 : NUM2DBL(vmax);

  normalize_range(map.heights, map.size, min, max);
  return self;
}

void load_heightmap() {
  VALUE mod, height_map;

  mod = rb_define_module("Worldgen");

  height_map = rb_define_class_under(mod, "HeightMap", rb_cObject);

  rb_define_private_method(height_map, "initialize_native", initialize_native, 1);
  rb_define_method(height_map, "num_points", num_points_wrapped, 0);
  rb_define_method(height_map, "each_height", each_height, 0);
  rb_define_method(height_map, "[]=", get_at, 2);
  rb_define_method(height_map, "[]=", set_at, 3);
  rb_define_method(height_map, "normalize!", normalize_hm, -1);

  HeightmapData = rb_define_class_under(height_map, "HeightmapData", rb_cObject);
}
