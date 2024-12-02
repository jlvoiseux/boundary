#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include <raylib.h>

#include "euler.h"
#include "gui.h"

int main(void) {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  GuiSetup(true);
  while (!WindowShouldClose()) {
    GuiBegin();
    GuiEnd();
  }

  GuiShutdown();
  return 0;
}
