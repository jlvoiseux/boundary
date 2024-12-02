#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "scan.h"

#include <stdio.h>

#include "defines.h"

void ListSolid(bdSolid* s) {
  bdFace* f;
  bdLoop* l;
  bdHalfEdge* he;

  printf("==========================\n");

  f = s->sfaces;
  while (f) {
    printf("face %d:\n", f->faceno);
    l = f->floops;
    while (l) {
      printf("loop:");
      he = l->ledg;
      do {
        printf(" %d: (%f %f %f)", he->vtx->vertexno, he->vtx->vcoord[0],
               he->vtx->vcoord[1], he->vtx->vcoord[2]);
      } while ((he = he->nxt) != l->ledg);
      printf("\n");
      l = l->nextl;
    }
    f = f->nextf;
  }
}

void ListSolidJson(bdSolid* s) {
  NULL_CHECK(s)

  printf("{\n");
  printf("  \"solid\": {\n");
  printf("    \"id\": %d,\n", s->solidno);
  printf("    \"faces\": [\n");

  bdFace* f = s->sfaces;
  while (f) {
    printf("      {\n");
    printf("        \"id\": %d,\n", f->faceno);
    printf("        \"loops\": [\n");

    bdLoop* l = f->floops;
    while (l) {
      printf("          {\n");
      printf("            \"vertices\": [\n");

      bdHalfEdge* he = l->ledg;
      do {
        printf("              {\n");
        printf("                \"id\": %d,\n", he->vtx->vertexno);
        printf("                \"coords\": [%f, %f, %f],\n",
               he->vtx->vcoord[0],
               he->vtx->vcoord[1],
               he->vtx->vcoord[2]);
        printf("                \"halfEdges\": {\n");
        printf("                  \"next\": %d,\n", he->nxt->vtx->vertexno);
        printf("                  \"prev\": %d,\n", he->prv->vtx->vertexno);
        if (he->edg) {
          printf("                  \"mate\": %d\n", MATE(he)->vtx->vertexno);
        } else {
          printf("                  \"mate\": null\n");
        }
        printf("                }\n");
        printf("              }%s\n",
               (he->nxt != l->ledg) ? "," : "");
        he = he->nxt;
      } while (he != l->ledg);

      printf("            ]\n");
      printf("          }%s\n", l->nextl ? "," : "");
      l = l->nextl;
    }

    printf("        ]\n");
    printf("      }%s\n", f->nextf ? "," : "");
    f = f->nextf;
  }

  printf("    ]\n");
  printf("  }\n");
  printf("}\n");
}

void ListNeighbors(bdVertex* v) {
  bdHalfEdge* adj;

  adj = v->vedge;
  if (adj) {
    do {
      printf("%d ", adj->nxt->vtx->vertexno);
    } while ((adj = MATE(adj)->nxt) != v->vedge);
  } else {
    printf("no adjacent vertices");
  }
  printf("\n");
}

bdSolid* GetSolid(Id sn) {
  bdSolid* s;
  for (s = firsts; s != NULL; s = s->nexts) {
    if (s->solidno == sn) {
      return s;
    }
  }
  return NULL;
}

bdFace* GetFace(bdSolid* s, Id fn) {
  bdFace* f;
  for (f = s->sfaces; f != NULL; f = f->nextf) {
    if (f->faceno == fn) {
      return f;
    }
  }
  return NULL;
}

bdHalfEdge* GetHalfEdge(bdFace* f, Id vn1, Id vn2) {
  bdLoop* l;
  bdHalfEdge* h;
  for (l = f->floops; l != NULL; l = l->nextl) {
    h = l->ledg;
    do {
      if (h->vtx->vertexno == vn1 && h->nxt->vtx->vertexno == vn2) {
        return h;
      }
    } while ((h = h->nxt) != l->ledg);
  }
  return NULL;
}

void GetMaxNames(Id sn) {
  bdSolid* s;
  bdVertex* v;
  bdFace* f;

  s = GetSolid(sn);
  for (v = s->sverts, maxv = 0; v != NULL; v = v->nextv) {
    if (v->vertexno > maxv) {
      maxv = v->vertexno;
    }
  }
  for (f = s->sfaces, maxf = 0; f != NULL; f = f->nextf) {
    if (f->faceno > maxf) {
      maxf = f->faceno;
    }
  }
}

int Match(bdHalfEdge* h1, bdHalfEdge* h2) {
  NULL_CHECK_RET(h1, 0)
  NULL_CHECK_RET(h2, 0)
  NULL_CHECK_RET(h1->vtx, 0)
  NULL_CHECK_RET(h2->vtx, 0)

  bdVertex* v1 = h1->vtx;
  bdVertex* v2 = h2->vtx;
  return (ABS(v1->vcoord[0] - v2->vcoord[0]) < EPS &&
          ABS(v1->vcoord[1] - v2->vcoord[1]) < EPS &&
          ABS(v1->vcoord[2] - v2->vcoord[2]) < EPS);
}
