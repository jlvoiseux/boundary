#pragma once

#include "prim.h"

void Sweep(bdFace* fac, float dx, float dy, float dz);
void Unsweep(bdFace* f);
void Rsweep(bdSolid* s, int nfaces, float xaxis, float yaxis, float zaxis);
void Merge(bdSolid* s1, bdSolid* s2);
void Glue(bdSolid* s1, bdSolid* s2, bdFace* f1, bdFace* f2);
void LoopGlue(bdFace* f);