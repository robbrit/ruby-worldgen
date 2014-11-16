#ifndef COMMON_H__
#define COMMON_H__

#define ARR(a, x, y) a[(x) * size + (y)]

typedef double * heightmap_points;

typedef struct {
  int size;
  heightmap_points heights;
} heightmap;

int num_points(int);
void normalize(double *, int);
void normalize_range(double *, int, double, double);

#endif // COMMON_H__
