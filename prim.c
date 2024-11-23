#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "prim.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>

#include "defines.h"

bdSolid* firsts = NULL;
Id maxs = 0;
Id maxf = 0;
Id maxv = 0;
double EPS = 1e-6;
double BIGEPS = 1e-4;

static unsigned node_size[] = {
    sizeof(bdSolid),
    sizeof(bdFace),
    sizeof(bdLoop),
    sizeof(bdEdge),
    sizeof(bdHalfEdge),
    sizeof(bdVertex),
    0,
};

bdNode* CreateNode(int what, bdNode* where) {
  bdNode* node;

  node = (bdNode*)malloc(node_size[what]);
  switch (what) {
    case SOLID:
      Append(SOLID, node, NULL);
      node->s.sfaces = NULL;
      node->s.sedges = NULL;
      node->s.sverts = NULL;
      break;
    case FACE:
      Append(FACE, node, where);
      node->f.floops = NULL;
      node->f.flout = NULL;
      break;
    case LOOP:
      Append(LOOP, node, where);
      break;
    case EDGE:
      Append(EDGE, node, where);
      break;
    case VERTEX:
      Append(VERTEX, node, where);
      node->v.vedge = NULL;
      break;
    default:
      break;
  }
  return node;
}

void DeleteNode(int what, bdNode* which, bdNode* where) {
  Remove(what, which, where);
  free(which);
  which = NULL;
}

void Append(int what, bdNode* which, bdNode* where) {
  NULL_CHECK(which)
  switch (what) {
    case SOLID:
      which->s.nexts = firsts;
      which->s.prevs = NULL;
      if (firsts) {
        firsts->prevs = (bdSolid*)which;
      }
      firsts = (bdSolid*)which;
      break;
    case FACE:
      which->f.nextf = where->s.sfaces;
      which->f.prevf = NULL;
      if (where->s.sfaces) {
        where->s.sfaces->prevf = (bdFace*)which;
      }
      where->s.sfaces = (bdFace*)which;
      which->f.fsolid = (bdSolid*)where;
      break;
    case LOOP:
      which->l.nextl = where->f.floops;
      which->l.prevl = NULL;
      if (where->f.floops) {
        where->f.floops->prevl = (bdLoop*)which;
      }
      where->f.floops = (bdLoop*)which;
      which->l.lface = (bdFace*)where;
      break;
    case EDGE:
      which->e.nexte = where->s.sedges;
      which->e.preve = NULL;
      if (where->s.sedges) {
        where->s.sedges->preve = (bdEdge*)which;
      }
      where->s.sedges = (bdEdge*)which;
      break;
    case VERTEX:
      which->v.nextv = where->s.sverts;
      which->v.prevv = NULL;
      if (where->s.sverts) {
        where->s.sverts->prevv = (bdVertex*)which;
      }
      where->s.sverts = (bdVertex*)which;
      break;
    default:
      assert(1);
  }
}

void Remove(int what, bdNode* which, bdNode* where) {
  switch (what) {
    case SOLID:
      if (which->s.prevs) {
        which->s.prevs->nexts = which->s.nexts;
      } else {
        firsts = which->s.nexts;
      }
      if (which->s.nexts) {
        which->s.nexts->prevs = which->s.prevs;
      }
      break;
    case FACE:
      if (which->f.prevf) {
        which->f.prevf->nextf = which->f.nextf;
      } else {
        which->f.fsolid->sfaces = which->f.nextf;
      }
      if (which->f.nextf) {
        which->f.nextf->prevf = which->f.prevf;
      }
      break;
    case LOOP:
      if (which->l.prevl) {
        which->l.prevl->nextl = which->l.nextl;
      } else {
        which->l.lface->floops = which->l.nextl;
      }
      if (which->l.nextl) {
        which->l.nextl->prevl = which->l.prevl;
      }
      break;
    case EDGE:
      if (which->e.preve) {
        which->e.preve->nexte = which->e.nexte;
      } else {
        NULL_CHECK(where)
        ((bdSolid*)where)->sedges = which->e.nexte;
      }
      if (which->e.nexte) {
        which->e.nexte->preve = which->e.preve;
      }
      break;
    case VERTEX:
      if (which->v.prevv) {
        which->v.prevv->nextv = which->v.nextv;
      } else {
        NULL_CHECK(where)
        ((bdSolid*)where)->sverts = which->v.nextv;
      }
      if (which->v.nextv) {
        which->v.nextv->prevv = which->v.prevv;
      }
      break;
    default:
      assert(1);
  }
}

bdHalfEdge* AppendHalfEdge(bdEdge* e, bdVertex* v, bdHalfEdge* where,
                           int sign) {
  bdHalfEdge* he;

  if (where->edg == NULL) {
    he = where;
  } else {
    he = (bdHalfEdge*)CreateNode(HALFEDGE, NULL);
    where->prv->nxt = he;
    he->prv = where->prv;
    where->prv = he;
    he->nxt = where;
  }

  he->edg = e;
  he->vtx = v;
  he->wloop = where->wloop;

  if (sign == PLUS) {
    e->he1 = he;
  } else {
    e->he2 = he;
  }

  return he;
}

bdHalfEdge* RemoveHalfEdge(bdHalfEdge* he) {
  NULL_CHECK_RET(he, NULL)
  bdHalfEdge* ret;

  if (he->edg == NULL) {
    DeleteNode(HALFEDGE, (bdNode*)he, NULL);
    ret = (bdHalfEdge*)NULL;
  } else if (he->nxt == he) {
    he->edg = NULL;
    ret = he;
  } else {
    ret = he->prv;
    he->prv->nxt = he->nxt;
    he->nxt->prv = he->prv;
    DeleteNode(HALFEDGE, (bdNode*)he, NULL);
  }

  return ret;
}