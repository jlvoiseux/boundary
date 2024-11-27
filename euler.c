#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "euler.h"

#include <stdio.h>

#include "defines.h"
#include "scan.h"

void Lmev(bdHalfEdge* he1, bdHalfEdge* he2, Id v, float x, float y, float z) {
  bdHalfEdge* he;
  bdVertex* newvertex;
  bdEdge* newedge;

  newedge = (bdEdge*)CreateNode(EDGE, (bdNode*)he1->wloop->lface->fsolid);
  newvertex = (bdVertex*)CreateNode(VERTEX, (bdNode*)he1->wloop->lface->fsolid);

  newvertex->vcoord[X] = x;
  newvertex->vcoord[Y] = y;
  newvertex->vcoord[Z] = z;
  newvertex->vcoord[3] = 1.0f;
  newvertex->vertexno = v;

  he = he1;
  while (he != he2) {
    he->vtx = newvertex;
    he = MATE(he)->nxt;
  }

  AppendHalfEdge(newedge, he2->vtx, he1, MINUS);
  AppendHalfEdge(newedge, newvertex, he2, PLUS);

  newvertex->vedge = he2->prv;
  he2->vtx->vedge = he2;
}

void Lkev(bdHalfEdge* he1, bdHalfEdge* he2) {
  NULL_CHECK(he1)
  NULL_CHECK(he2)
  NULL_CHECK(he1->vtx)
  NULL_CHECK(he2->vtx)
  NULL_CHECK(he1->edg)
  NULL_CHECK(he1->wloop)
  NULL_CHECK(he1->wloop->lface)
  NULL_CHECK(he1->wloop->lface->fsolid)

  bdVertex* v1 = he1->vtx;
  bdVertex* v2 = he2->vtx;
  bdEdge* killedge = he1->edg;
  bdSolid* solid = he1->wloop->lface->fsolid;
  bdHalfEdge* mate = MATE(he1);

  bdHalfEdge* current = he1;
  do {
    current->vtx = v2;
    current = MATE(current)->nxt;
  } while (current != he1);

  bdHalfEdge* h1 = RemoveHalfEdge(mate);  // Remove mate first
  bdHalfEdge* h2 = RemoveHalfEdge(he1);   // Then remove he1

  if (h1) {
    h1->vtx->vedge = h1;
  }
  if (h2) {
    h2->vtx->vedge = h2;
  }

  DeleteNode(EDGE, (bdNode*)killedge, (bdNode*)solid);
  DeleteNode(VERTEX, (bdNode*)v1, (bdNode*)solid);
}

bdFace* Lmef(bdHalfEdge* he1, bdHalfEdge* he2, Id f) {
  bdFace* newface;
  bdLoop* newloop;
  bdEdge* newedge;
  bdHalfEdge* he;
  bdHalfEdge* nhe1;
  bdHalfEdge* nhe2;
  bdHalfEdge* temp;

  newface = (bdFace*)CreateNode(FACE, (bdNode*)he1->wloop->lface->fsolid);
  newloop = (bdLoop*)CreateNode(LOOP, (bdNode*)newface);
  newedge = (bdEdge*)CreateNode(EDGE, (bdNode*)he1->wloop->lface->fsolid);
  newface->faceno = f;
  newface->flout = newloop;

  he = he1;
  while (he != he2) {
    he->wloop = newloop;
    he = he->nxt;
  }

  nhe2 = AppendHalfEdge(newedge, he1->vtx, he2, PLUS);
  nhe1 = AppendHalfEdge(newedge, he2->vtx, he1, MINUS);

  nhe1->prv->nxt = nhe2;
  nhe2->prv->nxt = nhe1;
  temp = nhe1->prv;
  nhe1->prv = nhe2->prv;
  nhe2->prv = temp;

  newloop->ledg = nhe1;
  he2->wloop->ledg = nhe2;

  return newface;
}

void Lkef(bdHalfEdge* he1, bdHalfEdge* he2) {
  NULL_CHECK(he1)
  NULL_CHECK(he2)
  NULL_CHECK(he1->edg)
  NULL_CHECK(he2->edg)
  NULL_CHECK(he1->wloop)
  NULL_CHECK(he2->wloop)
  NULL_CHECK(he1->wloop->lface)
  NULL_CHECK(he2->wloop->lface)

  bdLoop* l1 = he1->wloop;
  bdLoop* l2 = he2->wloop;
  bdFace* f2 = l2->lface;
  bdEdge* killedge = he1->edg;

  bdHalfEdge* curr = l2->ledg;
  do {
    curr->wloop = l1;
    curr = curr->nxt;
  } while (curr != l2->ledg);

  he1->prv->nxt = he2->nxt;
  he2->nxt->prv = he1->prv;
  he2->prv->nxt = he1->nxt;
  he1->nxt->prv = he2->prv;

  l1->ledg = he1->nxt;
  DeleteNode(HALFEDGE, (bdNode*)he1, NULL);
  DeleteNode(HALFEDGE, (bdNode*)he2, NULL);
  DeleteNode(EDGE, (bdNode*)killedge, (bdNode*)f2->fsolid);
  DeleteNode(LOOP, (bdNode*)l2, (bdNode*)f2);
  DeleteNode(FACE, (bdNode*)f2, (bdNode*)f2->fsolid);
}

bdLoop* Lkemr(bdHalfEdge* he1, bdHalfEdge* he2) {
  bdHalfEdge* h3;
  bdHalfEdge* h4;
  bdLoop* nl;
  bdLoop* ol;
  bdEdge* killedge;

  ol = he1->wloop;
  nl = (bdLoop*)CreateNode(LOOP, (bdNode*)ol->lface);
  killedge = he1->edg;

  h3 = he1->nxt;
  he1->nxt = he2->nxt;
  he2->nxt->prv = he1;
  he2->nxt = h3;
  h3->prv = he2;

  h4 = he2;
  do {
    h4->wloop = nl;
  } while ((h4 = h4->nxt) != he2);

  ol->ledg = h3 = RemoveHalfEdge(he1);
  nl->ledg = h4 = RemoveHalfEdge(he2);

  h3->vtx->vedge = h3->edg ? h3 : NULL;
  h4->vtx->vedge = h4->edg ? h4 : NULL;

  DeleteNode(EDGE, (bdNode*)killedge, (bdNode*)ol->lface->fsolid);

  return nl;
}

void Lmekr(bdHalfEdge* he1, bdHalfEdge* he2) {
  NULL_CHECK(he1)
  NULL_CHECK(he2)
  NULL_CHECK(he1->wloop)
  NULL_CHECK(he2->wloop)
  ASSERT(he1->wloop != he2->wloop)
  ASSERT(he1->wloop->lface == he2->wloop->lface)

  bdLoop* l1 = he1->wloop;
  bdLoop* l2 = he2->wloop;

  bdHalfEdge* temp = he2->nxt;
  he2->nxt = he1->nxt;
  he1->nxt->prv = he2;
  he1->nxt = temp;
  temp->prv = he1;

  bdHalfEdge* curr = he2;
  do {
    curr->wloop = l1;
    curr = curr->nxt;
  } while (curr != he2);

  l1->ledg = he1;

  DeleteNode(LOOP, (bdNode*)l2, (bdNode*)l2->lface);
}

void Lkfmrh(bdFace* fac1, bdFace* fac2) {
  NULL_CHECK(fac1)
  NULL_CHECK(fac2)
  NULL_CHECK(fac2->floops)
  ASSERT(fac2->floops->nextl == NULL)

  bdLoop* l2 = fac2->floops;
  bdLoop* l1 = fac1->floops;

  Remove(LOOP, (bdNode*)l2, (bdNode*)fac2);
  Append(LOOP, (bdNode*)l2, (bdNode*)fac1);

  l2->nextl = l1;
  l1->prevl = l2;
  fac1->floops = l2;

  DeleteNode(FACE, (bdNode*)fac2, (bdNode*)fac2->fsolid);
}

bdFace* Lmfkrh(bdLoop* l, Id f) {
  NULL_CHECK_RET(l, NULL)
  NULL_CHECK_RET(l->lface, NULL)
  ASSERT(l->lface->floops != l)

  bdFace* newface = (bdFace*)CreateNode(FACE, (bdNode*)l->lface->fsolid);
  newface->faceno = f;

  Remove(LOOP, (bdNode*)l, (bdNode*)l->lface);
  Append(LOOP, (bdNode*)l, (bdNode*)newface);

  newface->flout = l;
  newface->floops = NULL;

  return newface;
}

void Lringmv(bdLoop* l, bdFace* tofac, int inout) {
  NULL_CHECK(l)
  NULL_CHECK(tofac)

  if (l->lface == tofac) {
    if (inout == 0) {
      if (l == tofac->flout) {
        tofac->flout = NULL;
      }
    } else {
      tofac->flout = l;
    }
    return;
  }

  Remove(LOOP, (bdNode*)l, (bdNode*)l->lface);
  Append(LOOP, (bdNode*)l, (bdNode*)tofac);

  if (inout == 0) {
    if (l == l->lface->flout) {
      l->lface->flout = NULL;
    }
  } else {
    tofac->flout = l;
  }
}

bdSolid* Mvfs(Id s, Id f, Id v, float x, float y, float z) {
  bdSolid* newsolid;
  bdFace* newface;
  bdVertex* newvertex;
  bdHalfEdge* newhe;
  bdLoop* newloop;

  newsolid = (bdSolid*)CreateNode(SOLID, NULL);
  newface = (bdFace*)CreateNode(FACE, (bdNode*)newsolid);
  newloop = (bdLoop*)CreateNode(LOOP, (bdNode*)newface);
  newvertex = (bdVertex*)CreateNode(VERTEX, (bdNode*)newsolid);
  newhe = (bdHalfEdge*)CreateNode(HALFEDGE, NULL);

  newsolid->solidno = s;
  newface->faceno = f;
  newface->flout = newloop;
  newloop->ledg = newhe;
  newhe->wloop = newloop;
  newhe->nxt = newhe->prv = newhe;
  newhe->vtx = newvertex;
  newhe->edg = NULL;
  newvertex->vertexno = v;
  newvertex->vcoord[X] = x;
  newvertex->vcoord[Y] = y;
  newvertex->vcoord[Z] = z;
  newvertex->vcoord[3] = 1.0f;

  return newsolid;
}

void Kvfs(bdSolid* s) {
  NULL_CHECK(s)
  NULL_CHECK(s->sfaces)
  NULL_CHECK(s->sfaces->flout)
  NULL_CHECK(s->sfaces->flout->ledg)
  NULL_CHECK(s->sfaces->flout->ledg->vtx)

  bdFace* f = s->sfaces;
  bdLoop* l = f->flout;
  bdHalfEdge* he = l->ledg;
  bdVertex* v = he->vtx;

  DeleteNode(HALFEDGE, (bdNode*)he, NULL);
  DeleteNode(LOOP, (bdNode*)l, (bdNode*)f);
  DeleteNode(FACE, (bdNode*)f, (bdNode*)s);
  DeleteNode(VERTEX, (bdNode*)v, (bdNode*)s);
  DeleteNode(SOLID, (bdNode*)s, NULL);
}

int Mev(Id s, Id f1, Id f2, Id v1, Id v2, Id v3, Id v4, float x, float y,
        float z) {
  bdSolid* oldsolid;
  bdFace* oldface1;
  bdFace* oldface2;
  bdHalfEdge* he1;
  bdHalfEdge* he2;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "mev: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface1 = GetFace(oldsolid, f1)) == NULL) {
    fprintf(stderr, "mev: face %d not found in solid %d\n", f1, s);
    return ERROR;
  }

  if ((oldface2 = GetFace(oldsolid, f2)) == NULL) {
    fprintf(stderr, "mev: face %d not found in solid %d\n", f2, s);
    return ERROR;
  }

  if ((he1 = GetHalfEdge(oldface1, v1, v2)) == NULL) {
    fprintf(stderr, "mev: edge %d-%d not found in face %d\n", v1, v2, f1);
    return ERROR;
  }

  if ((he2 = GetHalfEdge(oldface2, v1, v3)) == NULL) {
    fprintf(stderr, "mev: edge %d-%d not found in face %d\n", v1, v3, f2);
    return ERROR;
  }

  Lmev(he1, he2, v4, x, y, z);
  return SUCCESS;
}

int Smev(Id s, Id f1, Id v1, Id v4, float x, float y, float z) {
  bdSolid* oldsolid;
  bdFace* oldface;
  bdHalfEdge* start = NULL;
  bdHalfEdge* he = NULL;
  int found = 0;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "smev: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface = GetFace(oldsolid, f1)) == NULL) {
    fprintf(stderr, "smev: face %d not found in solid %d\n", f1, s);
    return ERROR;
  }

  bdLoop* l = oldface->floops;
  while (l && !found) {
    start = l->ledg;
    do {
      if (start->vtx->vertexno == v1) {
        if (found) {
          fprintf(stderr, "smev: vertex %d appears multiple times in face %d\n",
                  v1, f1);
          return ERROR;
        }
        he = start;
        found = 1;
      }
      start = start->nxt;
    } while (start != l->ledg);
    l = l->nextl;
  }

  if (!found) {
    fprintf(stderr, "smev: vertex %d not found in face %d\n", v1, f1);
    return ERROR;
  }

  Lmev(he, he, v4, x, y, z);
  return SUCCESS;
}

int Kev(Id s, Id f, Id v1, Id v2) {
  bdSolid* oldsolid;
  bdFace* oldface;
  bdHalfEdge* he;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "kev: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface = GetFace(oldsolid, f)) == NULL) {
    fprintf(stderr, "kev: face %d not found in solid %d\n", f, s);
    return ERROR;
  }

  if ((he = GetHalfEdge(oldface, v1, v2)) == NULL) {
    fprintf(stderr, "kev: edge %d-%d not found in face %d\n", v1, v2, f);
    return ERROR;
  }

  Lkev(he, MATE(he));
  return SUCCESS;
}

int Mef(Id s, Id f1, Id v1, Id v2, Id v3, Id v4, Id f2) {
  bdSolid* oldsolid;
  bdFace* oldface;
  bdHalfEdge* he1;
  bdHalfEdge* he2;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "mef: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface = GetFace(oldsolid, f1)) == NULL) {
    fprintf(stderr, "mef: face %d not found in solid %d\n", f1, s);
    return ERROR;
  }

  if ((he1 = GetHalfEdge(oldface, v1, v2)) == NULL) {
    fprintf(stderr, "mef: edge %d-%d not found in face %d\n", v1, v2, f1);
    return ERROR;
  }

  if ((he2 = GetHalfEdge(oldface, v3, v4)) == NULL) {
    fprintf(stderr, "mef: edge %d-%d not found in face %d\n", v3, v4, f1);
    return ERROR;
  }

  Lmef(he1, he2, f2);
  return SUCCESS;
}

int Smef(Id s, Id f1, Id v1, Id v3, Id f2) {
  bdSolid* oldsolid;
  bdFace* oldface;
  bdHalfEdge* he1;
  bdHalfEdge* start;
  bdHalfEdge* he2;
  int found1 = 0;
  int found2 = 0;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "smef: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface = GetFace(oldsolid, f1)) == NULL) {
    fprintf(stderr, "smef: face %d not found in solid %d\n", f1, s);
    return ERROR;
  }

  bdLoop* l = oldface->floops;
  while (l && (!found1 || !found2)) {
    start = l->ledg;
    he1 = start;
    do {
      if (he1->vtx->vertexno == v1 && !found1) {
        found1 = 1;
      }
      if (he1->vtx->vertexno == v3 && !found2) {
        he2 = he1;
        found2 = 1;
      }
      he1 = he1->nxt;
    } while (he1 != start);
    l = l->nextl;
  }

  if (!found1 || !found2) {
    fprintf(stderr, "smef: vertices %d and %d not found in face %d\n", v1, v3,
            f1);
    return ERROR;
  }

  Lmef(he1, he2, f2);
  return SUCCESS;
}

int Kef(Id s, Id f, Id v1, Id v2) {
  bdSolid* oldsolid;
  bdFace* oldface;
  bdHalfEdge* he;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "kef: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface = GetFace(oldsolid, f)) == NULL) {
    fprintf(stderr, "kef: face %d not found in solid %d\n", f, s);
    return ERROR;
  }

  if ((he = GetHalfEdge(oldface, v1, v2)) == NULL) {
    fprintf(stderr, "kef: edge %d-%d not found in face %d\n", v1, v2, f);
    return ERROR;
  }

  Lkef(he, MATE(he));
  return SUCCESS;
}

int Ringmv(bdSolid* s, Id f1, Id f2, Id v1, Id v2, int inout) {
  NULL_CHECK_RET(s, ERROR)
  bdFace* face1;
  bdFace* face2;
  bdLoop* loop = NULL;

  if ((face1 = GetFace(s, f1)) == NULL) {
    fprintf(stderr, "ringmv: face %d not found in solid\n", f1);
    return ERROR;
  }

  if ((face2 = GetFace(s, f2)) == NULL) {
    fprintf(stderr, "ringmv: face %d not found in solid\n", f2);
    return ERROR;
  }

  if (v2 == 0) {
    loop = face1->flout;
  } else {
    bdHalfEdge* he = GetHalfEdge(face1, v1, v2);
    if (!he) {
      fprintf(stderr, "ringmv: edge %d-%d not found in face %d\n", v1, v2, f1);
      return ERROR;
    }
    loop = he->wloop;
  }

  if (!loop) {
    fprintf(stderr, "ringmv: could not find loop to move\n");
    return ERROR;
  }

  Lringmv(loop, face2, inout);
  return SUCCESS;
}

int Kemr(Id s, Id f, Id v1, Id v2) {
  bdSolid* oldsolid;
  bdFace* oldface;
  bdHalfEdge* he;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "kemr: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface = GetFace(oldsolid, f)) == NULL) {
    fprintf(stderr, "kemr: face %d not found in solid %d\n", f, s);
    return ERROR;
  }

  if ((he = GetHalfEdge(oldface, v1, v2)) == NULL) {
    fprintf(stderr, "kemr: edge %d-%d not found in face %d\n", v1, v2, f);
    return ERROR;
  }

  Lkemr(he, MATE(he));
  return SUCCESS;
}

int Mekr(Id s, Id f, Id v1, Id v2, Id v3, Id v4) {
  bdSolid* oldsolid;
  bdFace* oldface;
  bdHalfEdge* he1;
  bdHalfEdge* he2;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "mekr: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface = GetFace(oldsolid, f)) == NULL) {
    fprintf(stderr, "mekr: face %d not found in solid %d\n", f, s);
    return ERROR;
  }

  if ((he1 = GetHalfEdge(oldface, v1, v2)) == NULL) {
    fprintf(stderr, "mekr: edge %d-%d not found in face %d\n", v1, v2, f);
    return ERROR;
  }

  if ((he2 = GetHalfEdge(oldface, v3, v4)) == NULL) {
    fprintf(stderr, "mekr: edge %d-%d not found in face %d\n", v3, v4, f);
    return ERROR;
  }

  if (he1->wloop == he2->wloop) {
    fprintf(stderr, "mekr: edges must be in different loops\n");
    return ERROR;
  }

  Lmekr(he1, he2);
  return SUCCESS;
}

int Smekr(Id s, Id f, Id v1, Id v3) {
  bdSolid* oldsolid;
  bdFace* oldface;
  bdLoop* l;
  bdHalfEdge* start;
  bdHalfEdge* he1 = NULL;
  bdHalfEdge* he2 = NULL;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "smekr: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface = GetFace(oldsolid, f)) == NULL) {
    fprintf(stderr, "smekr: face %d not found in solid %d\n", f, s);
    return ERROR;
  }

  l = oldface->floops;
  while (l) {
    start = l->ledg;
    do {
      if (start->vtx->vertexno == v1 && he1 == NULL) {
        he1 = start;
      }
      if (start->vtx->vertexno == v3 && he2 == NULL) {
        he2 = start;
      }
      start = start->nxt;
    } while (start != l->ledg);
    l = l->nextl;
  }

  if (!he1 || !he2) {
    fprintf(stderr, "smekr: vertices %d and %d not found\n", v1, v3);
    return ERROR;
  }

  if (he1->wloop == he2->wloop) {
    fprintf(stderr, "smekr: vertices must be in different loops\n");
    return ERROR;
  }

  Lmekr(he1, he2);
  return SUCCESS;
}

int Kfmrh(Id s, Id f1, Id f2) {
  bdSolid* oldsolid;
  bdFace* face1;
  bdFace* face2;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "kfmrh: solid %d not found\n", s);
    return ERROR;
  }

  if ((face1 = GetFace(oldsolid, f1)) == NULL) {
    fprintf(stderr, "kfmrh: face %d not found in solid %d\n", f1, s);
    return ERROR;
  }

  if ((face2 = GetFace(oldsolid, f2)) == NULL) {
    fprintf(stderr, "kfmrh: face %d not found in solid %d\n", f2, s);
    return ERROR;
  }

  if (face2->floops->nextl != NULL) {
    fprintf(stderr, "kfmrh: face %d must have exactly one loop\n", f2);
    return ERROR;
  }

  Lkfmrh(face1, face2);
  return SUCCESS;
}

int Mfkrh(Id s, Id f1, Id v1, Id v2, Id f2) {
  bdSolid* oldsolid;
  bdFace* oldface;
  bdHalfEdge* he;
  bdLoop* l;

  if ((oldsolid = GetSolid(s)) == NULL) {
    fprintf(stderr, "mfkrh: solid %d not found\n", s);
    return ERROR;
  }

  if ((oldface = GetFace(oldsolid, f1)) == NULL) {
    fprintf(stderr, "mfkrh: face %d not found in solid %d\n", f1, s);
    return ERROR;
  }

  if ((he = GetHalfEdge(oldface, v1, v2)) == NULL) {
    fprintf(stderr, "mfkrh: edge %d-%d not found in face %d\n", v1, v2, f1);
    return ERROR;
  }

  if (he->wloop == oldface->flout) {
    fprintf(stderr, "mfkrh: edge %d-%d must be in inner loop\n", v1, v2);
    return ERROR;
  }

  l = he->wloop;
  bdFace* newface = Lmfkrh(l, f2);
  if (!newface) {
    return ERROR;
  }

  return SUCCESS;
}