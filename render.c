#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include <raylib.h>

#include "render.h"
#include "defines.h"
#include "scan.h"

void RenderSolid(const bdSolid* s) {
  NULL_CHECK(s)
  bdFace* f = s->sfaces;

  while (f) {
    bdLoop* l = f->floops;
    while (l) {
      bdHalfEdge* start = l->ledg;
      bdHalfEdge* he = start;

      do {
        Vector3 p1 = {
            he->vtx->vcoord[X],
            he->vtx->vcoord[Y],
            he->vtx->vcoord[Z]
        };
        Vector3 p2 = {
            he->nxt->vtx->vcoord[X],
            he->nxt->vtx->vcoord[Y],
            he->nxt->vtx->vcoord[Z]
        };

        DrawLine3D(p1, p2, BLUE);
        DrawSphere(p1, 0.1f, RED);

        he = he->nxt;
      } while (he != start);

      l = l->nextl;
    }
    f = f->nextf;
  }
}
