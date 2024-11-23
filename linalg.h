#pragma once

#include <math.h>
#include "defines.h"

typedef float bdVector[4];
typedef float bdMatrix[4][4];

void MatIdentity(bdMatrix m);
void MatRotate(bdMatrix m, float angle, float x, float y, float z);
void VecMultMatrix(bdVector v, const bdVector u, bdMatrix m);