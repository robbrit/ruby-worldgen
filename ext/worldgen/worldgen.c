#include <ruby.h>
#include <stdlib.h>
#include <time.h>

extern void load_heightmap();
extern void load_diamond_square();
extern void load_fbm();
extern void load_random_lattice();

void Init_worldgen() {
  srand((unsigned)time(NULL));

  load_heightmap();
  load_diamond_square();
  load_fbm();
  load_random_lattice();
}
