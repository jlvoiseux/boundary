#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include <raylib.h>

#include "gui.h"
#include "shape.h"

int main(void) {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  GuiSetup(true);

  Block(1, 2.0f, 2.0f, 2.0f);

  while (!WindowShouldClose()) {
    GuiBegin();
    GuiEnd();
  }

  GuiShutdown();
  return 0;
}
