#include "randomlattice.h"

#include <ruby.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

VALUE LatticeClass;

double cubic(int x0, int y0, int x1, int y1, double x, double y) {
  return 1.0;
}

/** This is a simple rotating hash with a bunch of prime numbers tossed in
 * to provide a suitable level of randomness.
 */
int simple_hash(int * is, int count) {
  // this contains a bunch of arbitrarily chosen prime numbers
  int i;
  unsigned int hash = 80238287;

  for (i = 0; i < count; i++){
    hash = (hash << 4) ^ (hash >> 28) ^ (is[i] * 5449 % 130651);
  }

  return hash % 75327403;
}

/**
 * Get the random value at point x, y in the lattice.
 */
double value_at(RandomLattice *lattice, double x, double y) {
  int ix = (int)x;
  int iy = (int)y;
  int is[] = {ix, iy, lattice->seed};

  srand(simple_hash(is, 3));
  // TODO: interpolation
  return (double)rand() / (RAND_MAX + 1.0);
}

/**
 * Get the value in the lattice at (x, y)
 */
VALUE value_at_wrapped(VALUE self, VALUE x, VALUE y) {
  RandomLattice * lattice;
  Data_Get_Struct(self, RandomLattice, lattice);

  return DBL2NUM(value_at(lattice, NUM2DBL(x), NUM2DBL(y)));
}

/**
 * Initialize the lattice.
 *
 * Arguments;
 * * width - The width of the lattice
 * * height - The height of the lattice
 * * seed (optional) - The seed to use for the lattice.
 */
VALUE init_lattice(int argc, VALUE *argv, VALUE self) {
  RandomLattice * lattice;
  VALUE vwidth, vheight, vseed;
  int width, height, seed;

  rb_scan_args(argc, argv, "21", &vwidth, &vheight, &vseed);
  width = FIX2INT(vwidth);
  height = FIX2INT(vheight);
  printf("%d, %d\n", width, height);
  seed = vseed == Qnil ? (int)time(NULL) : FIX2INT(vseed);

  Data_Get_Struct(self, RandomLattice, lattice);
  lattice->height = height;
  lattice->width = width;
  lattice->seed = seed;
  lattice->dimension = 2;
  lattice->interpolate = cubic;

  return self;
}

/**
 * Create a new lattice.
 *
 * See RandomLattice#initialize for arguments.
 */
VALUE new_lattice(int argc, VALUE *argv, VALUE class) {
  RandomLattice * lattice;
  VALUE result = Data_Make_Struct(LatticeClass, RandomLattice, 0, free, lattice);
  rb_obj_call_init(result, argc, argv);
  return result;
}

/**
 * Get the width of the lattice
 */
VALUE get_width(VALUE self) {
  RandomLattice * lattice;
  Data_Get_Struct(self, RandomLattice, lattice);
  return INT2FIX(lattice->width);
}

/**
 * Get the height of the lattice
 */
VALUE get_height(VALUE self) {
  RandomLattice * lattice;
  Data_Get_Struct(self, RandomLattice, lattice);
  return INT2FIX(lattice->height);
}

/**
 * Iterate across points within the lattice.
 *
 * Example:
 *
 * each_point(0, 0, 100, 100, 0.5, 0.5) do |x, y, value|
 *  # do something with each value
 * end
 */
VALUE each_point(VALUE self, VALUE vminx, VALUE vminy, VALUE vmaxx, VALUE vmaxy, VALUE vstepx, VALUE vstepy) {
  double minx, miny, maxx, maxy, stepx, stepy;
  RandomLattice * lattice;
  double x, y;
  VALUE args = rb_ary_new2(3);

  Data_Get_Struct(self, RandomLattice, lattice);

  minx = NUM2DBL(vminx);
  miny = NUM2DBL(vminy);
  maxx = NUM2DBL(vmaxx);
  maxy = NUM2DBL(vmaxy);
  stepx = NUM2DBL(vstepx);
  stepy = NUM2DBL(vstepy);

  for (x = minx; x < maxx; x += stepx) {
    rb_ary_store(args, 0, DBL2NUM(x));
    for (y = miny; y < maxy; y += stepy) {
      rb_ary_store(args, 1, DBL2NUM(y));
      rb_ary_store(args, 2, DBL2NUM(value_at(lattice, x, y)));
      rb_yield(args);
    }
  }

  return Qnil;
}

void load_random_lattice() {
  VALUE mod;

  mod = rb_define_module("Worldgen");

  LatticeClass = rb_define_class_under(mod, "RandomLattice", rb_cObject);

  rb_define_singleton_method(LatticeClass, "new", new_lattice, -1);
  rb_define_method(LatticeClass, "initialize", init_lattice, -1);

  rb_define_method(LatticeClass, "[]", value_at_wrapped, 2);
  rb_define_method(LatticeClass, "width", get_width, 0);
  rb_define_method(LatticeClass, "height", get_height, 0);
  rb_define_method(LatticeClass, "each_point", each_point, 6);
}
