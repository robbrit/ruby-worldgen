#include "common.h"

/**
 * Convert an array of doubles into normalized doubles between 0 and 1
 **/
void normalize(double * values, int size) {
  double min = values[0],
         max = values[0];

  int x;

  for (x = 1; x < num_points(size); x++) {
    if (values[x] > max) {
      max = values[x];
    }
    if (values[x] < min) {
      min = values[x];
    }
  }

  for (x = 0; x < num_points(size); x++) {
    values[x] = (values[x] - min) / (max - min);
  }
}

/** Get the number of points for a heightmap of a given size. This will change
 * later when we move from beyond just a simple square geometry.
 */
int num_points(int size) {
  return size * size;
}
