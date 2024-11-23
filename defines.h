#pragma once

#include <assert.h>
#include <stddef.h>

#define ERROR -1
#define SUCCESS -2
#define PI 3.14159265358979323846f

#define SOLID 0
#define FACE 1
#define LOOP 2
#define HALFEDGE 3
#define EDGE 4
#define VERTEX 5

#define X 0
#define Y 1
#define Z 2

#define PLUS 0
#define MINUS 1

#define MATE(he) (((he) == (he)->edg->he1) ? (he)->edg->he2 : (he)->edg->he1)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define ABS(x) (((x) > 0.0) ? (x) : -(x))

#define ASSERT(x) \
  { assert(x); }
#define NULL_CHECK(x) \
  if ((x) == NULL) {  \
    assert(1);        \
    return;           \
  }
#define NULL_CHECK_RET(x, retval) \
  if ((x) == NULL) {              \
    assert(1);                    \
    return retval;                \
  }
