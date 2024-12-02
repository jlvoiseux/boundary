#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "shape.h"

#include <math.h>

#include "euler.h"
#include "scan.h"
#include "transform.h"

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

bdSolid* Circle(Id sn, float cx, float cy, float rad, float h, int n) {
  bdSolid* s;

  s = Mvfs(sn, 1, 1, cx + rad, cy, h);
  Arc(sn, 1, 1, cx, cy, rad, h, 0.0f, ((float)(n - 1) * 360.0f / (float)n),n - 1);
  Smef(sn, 1, 1, n, 2);
  return s;
}

bdSolid* Block(Id sn, float dx, float dy, float dz) {
  bdSolid* s;

  s = Mvfs(sn, 1, 1, 0.0f, 0.0f, 0.0f);
  Smev(sn, 1, 1, 2, dx, 0.0f, 0.0f);
  Smev(sn, 1, 2, 3, dx, dy, 0.0f);
  Smev(sn, 1, 3, 4, 0.0f, dy, 0.0f);
  Smef(sn, 1, 1, 4, 2);
  Sweep(GetFace(s, 2), 0.0f, 0.0f, dz);
  return s;
}

bdSolid* Cyl(Id sn, float rad, float h, int n) {
  bdSolid* s;

  s = Circle(sn, 0.0f, 0.0f, rad, 0.0f, n);
  Sweep(GetFace(s, 2), 0.0f, 0.0f, h);
  return s;
}

bdSolid* Ball(Id sn, float rad, int nver, int nhor) {
  bdSolid* s;

  s = Mvfs(sn, 1, 1, -rad, 0.0f, 0.0f);
  Arc(sn, 1, 1, 0.0f, 0.0f, rad, 0.0f, 180.0f, 0.0f, nver);
  Rsweep(s, nhor, 1.0f, 0.0f, 0.0f);
  return s;
}

bdSolid* Torus(Id sn, float r1, float r2, int nf1, int nf2) {
  bdSolid* s;

  float circle_radius = (r2-r1)/2.0f;
  s = Circle(sn, 0.0f, r1 + circle_radius, circle_radius, 0.0f, nf2);
  Rsweep(s, nf1, 1.0f, 0.0f, 0.0f);
  return s;
}