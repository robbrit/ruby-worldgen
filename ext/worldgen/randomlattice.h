#ifndef RANDOM_LATTICE_H__
#define RANDOM_LATTICE_H__

typedef double (*interp_function)(double, double, double);

typedef struct {
  int seed;
  int dimension;
  int width, height;

  // Method to interpolate between two points
  interp_function interpolate;
} RandomLattice;

void create_lattice(RandomLattice *lattice, int width, int height, int seed);
double value_at(RandomLattice *lattice, double x, double y);

#endif // RANDOM_LATTICE_H__
