#include <ruby.h>
#include <stdlib.h>
#include <time.h>

extern void load_heightmap();

void Init_worldgen() {
  srand((unsigned)time(NULL));

  load_heightmap();
}
