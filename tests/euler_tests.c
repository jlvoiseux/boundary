#include "unity.h"
#include "../euler.h"
#include "../scan.h"
#include "../defines.h"

static bdSolid* test_solid;
static bdFace* test_face;
static bdLoop* test_loop;
static bdHalfEdge* test_he;
static bdVertex* test_vertex;

void setUp(void) {
  firsts = NULL;
  maxs = maxf = maxv = 0;
  test_solid = NULL;
  test_face = NULL;
  test_loop = NULL;
  test_he = NULL;
  test_vertex = NULL;
}

void tearDown(void) {
  while (firsts) {
    Kvfs(firsts);
  }
}

void test_Mvfs(void) {
  test_solid = Mvfs(1, 1, 1, 0.0f, 0.0f, 0.0f);
  
  TEST_ASSERT_NOT_NULL(test_solid);
  TEST_ASSERT_EQUAL_INT(1, test_solid->solidno);
  TEST_ASSERT_NOT_NULL(test_solid->sfaces);
  TEST_ASSERT_EQUAL_INT(1, test_solid->sfaces->faceno);
  TEST_ASSERT_NOT_NULL(test_solid->sfaces->flout);
  TEST_ASSERT_NOT_NULL(test_solid->sfaces->flout->ledg);
  TEST_ASSERT_EQUAL_FLOAT(0.0f, test_solid->sfaces->flout->ledg->vtx->vcoord[0]);
  TEST_ASSERT_EQUAL_FLOAT(0.0f, test_solid->sfaces->flout->ledg->vtx->vcoord[1]);
  TEST_ASSERT_EQUAL_FLOAT(0.0f, test_solid->sfaces->flout->ledg->vtx->vcoord[2]);
}

void test_Mev(void) {
  test_solid = Mvfs(1, 1, 1, 0.0f, 0.0f, 0.0f);
  TEST_ASSERT_EQUAL_INT(SUCCESS, Smev(1, 1, 1, 2, 1.0f, 0.0f, 0.0f));
  
  bdVertex* v2 = test_solid->sfaces->flout->ledg->nxt->vtx;
  TEST_ASSERT_NOT_NULL(v2);
  TEST_ASSERT_EQUAL_INT(2, v2->vertexno);
  TEST_ASSERT_EQUAL_FLOAT(1.0f, v2->vcoord[0]);
  TEST_ASSERT_EQUAL_FLOAT(0.0f, v2->vcoord[1]);
  TEST_ASSERT_EQUAL_FLOAT(0.0f, v2->vcoord[2]);
}

void test_Mef(void) {
  test_solid = Mvfs(1, 1, 1, 0.0f, 0.0f, 0.0f);
  Smev(1, 1, 1, 2, 1.0f, 0.0f, 0.0f);
  Smev(1, 1, 2, 3, 1.0f, 1.0f, 0.0f);
  Smev(1, 1, 3, 4, 0.0f, 1.0f, 0.0f);
  TEST_ASSERT_EQUAL_INT(SUCCESS, Smef(1, 1, 1, 4, 2));
  
  bdFace* f2 = GetFace(test_solid, 2);
  TEST_ASSERT_NOT_NULL(f2);
  TEST_ASSERT_EQUAL_INT(2, f2->faceno);
}

void test_Kev(void) {
  test_solid = Mvfs(1, 1, 1, 0.0f, 0.0f, 0.0f);
  Smev(1, 1, 1, 2, 1.0f, 0.0f, 0.0f);
  
  TEST_ASSERT_EQUAL_INT(SUCCESS, Kev(1, 1, 1, 2));
  
  bdHalfEdge* he = test_solid->sfaces->flout->ledg;
  TEST_ASSERT_NULL(he->edg);
}

void test_Kef(void) {
  test_solid = Mvfs(1, 1, 1, 0.0f, 0.0f, 0.0f);
  Smev(1, 1, 1, 2, 1.0f, 0.0f, 0.0f);
  Smev(1, 1, 2, 3, 1.0f, 1.0f, 0.0f);
  Smev(1, 1, 3, 4, 0.0f, 1.0f, 0.0f);
  Smef(1, 1, 1, 4, 2);
  
  TEST_ASSERT_EQUAL_INT(SUCCESS, Kef(1, 1, 1, 4));
  TEST_ASSERT_NULL(GetFace(test_solid, 2));
}

void test_Others(void) {
  test_solid = Mvfs(1, 1, 1, 0.0f, 0.0f, 0.0f);
  Smev(1, 1, 1, 2, 0.0f, 3.0f, 0.0f);
  Smev(1, 1, 2, 3, 1.0f, 2.0f, 0.0f);
  Smev(1, 1, 3, 4, 1.0f, 1.0f, 0.0f);
  Smef(1, 1, 1, 4, 2);

  Smev(1, 2, 2, 5, 3.0f, 3.0f, 0.0f);
  Smev(1, 2, 5, 6, 2.0f, 2.0f, 0.0f);
  Smef(1, 2, 3, 6, 3);

  Smev(1, 3, 5, 7, 3.0f, 0.0f, 0.0f);
  Smev(1, 3, 7, 8, 2.0f, 1.0f, 0.0f);
  Smef(1, 3, 6, 8, 4);
  Smef(1, 4, 4, 8, 5);
  Smef(1, 5, 1, 7, 6);

  Kef(1, 1, 3, 2);
  Kef(1, 1, 6, 5);
  Kef(1, 1, 8, 7);
  Kef(1, 1, 1, 4);

  TEST_ASSERT_EQUAL_INT(SUCCESS, Kfmrh(1, 4, 6));
  TEST_ASSERT_EQUAL_INT(SUCCESS, Smekr(1, 4, 1, 4));
  TEST_ASSERT_EQUAL_INT(SUCCESS, Kemr(1, 4, 4, 1));

  int face_count = 0;
  bdFace* f = test_solid->sfaces;
  while (f) {
    face_count++;
    f = f->nextf;
  }
  TEST_ASSERT_EQUAL_INT(1, face_count);

  TEST_ASSERT_EQUAL_INT(SUCCESS, Mfkrh(1, 4, 4, 8, 5));

  face_count = 0;
  f = test_solid->sfaces;
  while (f) {
    face_count++;
    f = f->nextf;
  }
  TEST_ASSERT_EQUAL_INT(2, face_count);
}
int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_Mvfs);
  RUN_TEST(test_Mev);
  RUN_TEST(test_Mef);
  RUN_TEST(test_Kev);
  RUN_TEST(test_Kef);
  RUN_TEST(test_Others);
  return UNITY_END();
}