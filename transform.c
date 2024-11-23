#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "transform.h"

#include "defines.h"
#include "euler.h"
#include "scan.h"

void Sweep(bdFace* fac, float dx, float dy, float dz) {
  bdLoop* l;
  bdHalfEdge* first;
  bdHalfEdge* scan;
  bdVertex* v;

  GetMaxNames(fac->fsolid->solidno);
  l = fac->floops;

  while (l) {
    first = l->ledg;
    scan = first->nxt;
    v = scan->vtx;

    Lmev(scan, scan, ++maxv, v->vcoord[X] + dx, v->vcoord[Y] + dy,
         v->vcoord[Z] + dz);

    while (scan != first) {
      v = scan->nxt->vtx;
      Lmev(scan->nxt, scan->nxt, ++maxv, v->vcoord[X] + dx, v->vcoord[Y] + dy,
           v->vcoord[Z] + dz);
      Lmef(scan->prv, scan->nxt->nxt, ++maxf);
      scan = MATE(scan->nxt)->nxt;
    }

    Lmef(scan->prv, scan->nxt->nxt, ++maxf);
    l = l->nextl;
  }
}

void Unsweep(bdFace* f) {
  NULL_CHECK(f)
  bdLoop* l = f->floops;
  bdLoop* nextl;
  bdHalfEdge* first;
  bdHalfEdge* scan;
  bdHalfEdge* todelete;

  while (l) {
    nextl = l->nextl;
    first = l->ledg;
    scan = first;

    do {
      todelete = MATE(scan)->nxt;
      if (todelete != first) {
        Kef(f->fsolid->solidno, MATE(scan)->wloop->lface->faceno,
            todelete->vtx->vertexno, todelete->nxt->vtx->vertexno);
        Kev(f->fsolid->solidno, f->faceno, scan->vtx->vertexno,
            MATE(scan)->vtx->vertexno);
      }
      scan = scan->nxt;
    } while (scan != first);

    l = nextl;
  }
}

void Rsweep(bdSolid* s, int nfaces) {
  NULL_CHECK(s)
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

  GetMaxNames(s->solidno);

  if (s->sfaces->nextf) {
    closed_figure = 1;
    h = s->sfaces->floops->ledg;
    Lmev(h, MATE(h)->nxt, ++maxv, h->vtx->vcoord[X], h->vtx->vcoord[Y],
         h->vtx->vcoord[Z]);
    Lkef(h->prv, MATE(h->prv));
    headf = s->sfaces;
  } else {
    closed_figure = 0;
  }

  first = s->sfaces->floops->ledg;
  while (first->edg != first->nxt->edg) {
    first = first->nxt;
  }
  last = first->nxt;
  while (last->edg != last->nxt->edg) {
    last = last->nxt;
  }
  cfirst = first;

  MatIdentity(m);
  MatRotate(m, (360.0f / (float)nfaces), 0.0f, 0.0f, 1.0f);

  while (--nfaces) {
    VecMultMatrix(v, cfirst->nxt->vtx->vcoord, m);
    Lmev(cfirst->nxt, cfirst->nxt, ++maxv, v[0], v[1], v[2]);

    scan = cfirst->nxt;
    while (scan != last->nxt) {
      VecMultMatrix(v, scan->prv->vtx->vcoord, m);
      Lmev(scan->prv, scan->prv, ++maxv, v[0], v[1], v[2]);
      Lmef(scan->prv->prv, scan->nxt, ++maxf);
      scan = MATE(scan->nxt)->nxt;
    }

    last = scan;
    cfirst = MATE(cfirst->nxt->nxt);
  }

  tailf = Lmef(first->nxt, MATE(first), ++maxf);
  while (cfirst != scan) {
    Lmef(cfirst, cfirst->nxt->nxt->nxt, ++maxf);
    cfirst = MATE(cfirst->prv)->prv;
  }

  if (closed_figure) {
    Lkfmrh(headf, tailf);
    LoopGlue(headf);
  }
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

void Glue(bdSolid* s1, bdSolid* s2, bdFace* f1, bdFace* f2) {
  NULL_CHECK(s1)
  NULL_CHECK(s2)
  NULL_CHECK(f1)
  NULL_CHECK(f2)

  Merge(s1, s2);
  Lkfmrh(f1, f2);
  LoopGlue(f1);
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