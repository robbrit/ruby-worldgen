#include <ruby.h>
#include <stdlib.h>
#include <time.h>

extern void load_heightmap();
extern void load_diamond_square();

void Init_worldgen() {
  srand((unsigned)time(NULL));

  load_heightmap();
  load_diamond_square();
}
