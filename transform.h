#pragma once

#include "prim.h"

void Merge(bdSolid* s1, bdSolid* s2);
void LoopGlue(bdFace* f);

void Sweep(Id s, Id f, float dx, float dy, float dz);
void Rsweep(Id s, int nfaces, float xaxis, float yaxis, float zaxis);
void Arc(Id s, Id f, Id v, float cx, float cy, float rad, float h, float phi1, float phi2, int n);
void Circle(Id s, float cx, float cy, float rad, float h, int n);