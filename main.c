#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include <raylib.h>

#include "gui.h"
#include "scan.h"
#include "shape.h"

int main(void) {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  GuiSetup(true);

  //bdSolid* block = Block(1, 2.0f, 2.0f, 2.0f);
  //bdSolid* cyl = Cyl(1, 2.0f, 2.0f, 8);
  //bdSolid* ball = Ball(1, 2.0f, 8, 8);
  bdSolid* torus = Torus(1, 1.5f, 2.0f, 8, 8);

  while (!WindowShouldClose()) {
    GuiBegin();
    GuiEnd();
  }

  GuiShutdown();
  return 0;
}
