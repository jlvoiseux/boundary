#pragma once

#include "prim.h"

void Arc(Id s, Id f, Id v, float cx, float cy, float rad, float h, float phi1,
         float phi2, int n);
bdSolid* Circle(Id sn, float cx, float cy, float rad, float h, int n);
bdSolid* Cube(Id sn, float dx, float dy, float dz);
bdSolid* Cyl(Id sn, float rad, float h, int n);
bdSolid* Sphere(Id sn, float rad, int nver, int nhor);
bdSolid* Torus(Id sn, float r1, float r2, int nf1, int nf2);