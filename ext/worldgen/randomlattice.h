#ifndef RANDOM_LATTICE_H__
#define RANDOM_LATTICE_H__

typedef double (*interp_function)(int x0, int y0, int x1, int y1, double x, double y);

typedef struct {
  int seed;
  int dimension;
  int width, height;

  // Method to interpolate between two points
  interp_function interpolate;
} RandomLattice;

double value_at(RandomLattice *lattice, double x, double y);

#endif // RANDOM_LATTICE_H__
