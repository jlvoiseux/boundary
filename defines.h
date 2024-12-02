#pragma once

#include <assert.h>
#include <stddef.h>

#define ERROR -1
#define SUCCESS -2
#define PI 3.14159265358979323846f

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
