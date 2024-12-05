#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "transform.h"

#include "defines.h"
#include "euler.h"
#include "scan.h"

void Sweep(Id s, Id f, float dx, float dy, float dz) {
  bdLoop* l;
  bdHalfEdge* first;
  bdHalfEdge* scan;
  bdVertex* v;

  bdSolid* sol = GetSolid(s);
  bdFace* fac = GetFace(sol, f);

  GetMaxNames(fac->fsolid->solidno);
  l = fac->floops;

  while (l) {
    first = l->ledg;
    scan = first->nxt;
    v = scan->vtx;

    Lmev(scan, scan, ++maxv, v->vcoord[0] + dx, v->vcoord[1] + dy,
         v->vcoord[2] + dz);

    while (scan != first) {
      v = scan->nxt->vtx;
      Lmev(scan->nxt, scan->nxt, ++maxv, v->vcoord[0] + dx, v->vcoord[1] + dy,
           v->vcoord[2] + dz);
      Lmef(scan->prv, scan->nxt->nxt, ++maxf);
      scan = MATE(scan->nxt)->nxt;
    }

    Lmef(scan->prv, scan->nxt->nxt, ++maxf);
    l = l->nextl;
  }
}

void Rsweep(Id s, int nfaces, float xaxis, float yaxis, float zaxis) {
  bdHalfEdge* first;
  bdHalfEdge* cfirst;
  bdHalfEdge* last;
  bdHalfEdge* scan;
  bdHalfEdge* h;
  bdFace* headf;
  bdFace* tailf;
  bdMatrix m;
  bdVector v;
  int closed_figure;

  bdSolid* sol = GetSolid(s);
  NULL_CHECK(sol)

  if (sol->sfaces->nextf) {
    closed_figure = 1;
    h = sol->sfaces->floops->ledg;
    Lmev(h, MATE(h)->nxt, ++maxv, h->vtx->vcoord[0], h->vtx->vcoord[1],
         h->vtx->vcoord[2]);
    Lkef(h->prv, MATE(h->prv));
    headf = sol->sfaces;
  } else {
    closed_figure = 0;
  }

  GetMaxNames(s);
  first = sol->sfaces->floops->ledg;
  while (first->edg != first->nxt->edg) {
    first = first->nxt;
  }
  last = first->nxt;
  while (last->edg != last->nxt->edg) {
    last = last->nxt;
  }
  cfirst = first;

  MatIdentity(m);
  MatRotate(m, (360.0f / (float)nfaces), 1.0f, 0.0f, 0.0f);

  while (--nfaces) {
    VecMultMatrix(v, cfirst->nxt->vtx->vcoord, m);
    Lmev(cfirst->nxt, cfirst->nxt, ++maxv, v[0], v[1], v[2]);
    scan = cfirst->nxt;
    while (scan != last->nxt) {
      VecMultMatrix(v, scan->prv->vtx->vcoord, m);
      Lmev(scan->prv, scan->prv, ++maxv, v[0], v[1], v[2]);
      Lmef(scan->prv->prv, scan->nxt, ++maxf);
      scan = MATE(scan->nxt->nxt);
    }

    last = scan;
    cfirst = MATE(cfirst->nxt->nxt);
  }

  tailf = Lmef(cfirst->nxt, MATE(first), ++maxf);
  while (cfirst != scan) {
    Lmef(cfirst, cfirst->nxt->nxt->nxt, ++maxf);
    cfirst = MATE(cfirst->prv)->prv;
  }

  if (closed_figure) {
    Lkfmrh(headf, tailf);
    LoopGlue(headf);
  }
}

void Arc(Id s, Id f, Id v, float cx, float cy, float rad, float h, float phi1,
         float phi2, int n) {
  float x, y, angle, inc;
  Id prev;
  int i;

  angle = phi1 * PI / 180.0f;
  inc = (phi2 - phi1) * PI / (180.0f * (float)n);
  prev = v;
  GetMaxNames(s);

  for (i = 0; i < n; i++) {
    angle += inc;
    x = cx + cosf(angle) * rad;
    y = cy + sinf(angle) * rad;
    Smev(s, f, prev, ++maxv, x, y, h);
    prev = maxv;
  }
}

void Circle(Id s, float cx, float cy, float rad, float h, int n) {
  Arc(s, 1, 1, cx, cy, rad, h, 0.0f, ((float)(n - 1) * 360.0f / (float)n),n - 1);
  Smef(s, 1, 1, n, 2);
}


void Merge(bdSolid* s1, bdSolid* s2) {
  NULL_CHECK(s1)
  NULL_CHECK(s2)

  while (s2->sfaces) {
    Remove(FACE, (bdNode*)s2->sfaces, (bdNode*)s2);
    Append(FACE, (bdNode*)s2->sfaces, (bdNode*)s1);
  }
  while (s2->sedges) {
    Remove(EDGE, (bdNode*)s2->sedges, (bdNode*)s2);
    Append(EDGE, (bdNode*)s2->sedges, (bdNode*)s1);
  }
  while (s2->sverts) {
    Remove(VERTEX, (bdNode*)s2->sverts, (bdNode*)s2);
    Append(VERTEX, (bdNode*)s2->sverts, (bdNode*)s1);
  }
  DeleteNode(SOLID, (bdNode*)s2, NULL);
}

void LoopGlue(bdFace* fac) {
  NULL_CHECK(fac)
  NULL_CHECK(fac->floops)
  NULL_CHECK(fac->floops->nextl)

  bdHalfEdge* h1;
  bdHalfEdge* h2;
  bdHalfEdge* h1next;

  h1 = fac->floops->ledg;
  h2 = fac->floops->nextl->ledg;

  while (!Match(h1, h2)) {
    h2 = h2->nxt;
  }

  Lmekr(h1, h2);
  Lkev(h1->prv, h2->prv);

  while (h1->nxt != h2) {
    h1next = h1->nxt;
    Lmef(h1->nxt, h1->prv, -1);
    Lkev(h1->nxt, MATE(h1->nxt));
    Lkef(MATE(h1), h1);
    h1 = h1next;
  }

  Lkef(MATE(h1), h1);
}