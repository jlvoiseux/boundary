#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "linalg.h"

void MatIdentity(bdMatrix m) {
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      m[i][j] = (i == j) ? 1.0f : 0.0f;
    }
  }
}

void MatRotate(bdMatrix m, float angle, float x, float y, float z) {
  float c = cosf(angle * PI / 180.0f);
  float s = sinf(angle * PI / 180.0f);
  float t = 1.0f - c;

  m[0][0] = t * x * x + c;
  m[0][1] = t * x * y - s * z;
  m[0][2] = t * x * z + s * y;
  m[0][3] = 0.0f;

  m[1][0] = t * x * y + s * z;
  m[1][1] = t * y * y + c;
  m[1][2] = t * y * z - s * x;
  m[1][3] = 0.0f;

  m[2][0] = t * x * z - s * y;
  m[2][1] = t * y * z + s * x;
  m[2][2] = t * z * z + c;
  m[2][3] = 0.0f;

  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 0.0f;
  m[3][3] = 1.0f;
}

void VecMultMatrix(bdVector v, const bdVector u, bdMatrix m) {
  int i, j;
  float sum;
  bdVector temp;

  for (i = 0; i < 4; i++) {
    sum = 0.0f;
    for (j = 0; j < 4; j++) {
      sum += u[j] * m[j][i];
    }
    temp[i] = sum;
  }

  for (i = 0; i < 4; i++) {
    v[i] = temp[i];
  }
}
