#pragma once

#include "prim.h"

//
// Low-level operators
//

// Edge splitting operator (Loop Make Edge Vertex)
void Lmev(bdHalfEdge* he1, bdHalfEdge* he2, Id v, float x, float y, float z);
// Inverse (Loop Kill Edge Vertex)
void Lkev(bdHalfEdge* he1, bdHalfEdge* he2);

// Face splitting operator (Loop Make Face Vertex)
bdFace* Lmef(bdHalfEdge* he1, bdHalfEdge* he2, Id f);
// Inverse (Loop Kill Edge Face)
void Lkef(bdHalfEdge* he1, bdHalfEdge* he2);
// Loop movement addendum (Loop Ring Move)
void Lringmv(bdLoop* l, bdFace* tofac, int inout);

// Loop splitting operator (Loop Kill Edge Make Ring)
bdLoop* Lkemr(bdHalfEdge* he1, bdHalfEdge* he2);
// Inverse (Loop Make Edge Kill Ring)
void Lmekr(bdHalfEdge* he1, bdHalfEdge* he2);

// Face merging by creating an inner loop (Loop Kill Face Make Ring Hole)
void Lkfmrh(bdFace* fac1, bdFace* fac2);
// Inverse (Loop Make Face Kill Ring Hole)
bdFace* Lmfkrh(bdLoop* l, Id f);

//
// High-level operators
//

// Initialization operator (Make Vertex Face Solid)
bdSolid* Mvfs(Id s, Id f, Id v, float x, float y, float z);
// Inverse (Loop Kill Vertex Face Solid)
void Kvfs(bdSolid* s);

// Edge splitting operator (Make Edge Vertex)
int Mev(Id s, Id f1, Id f2, Id v1, Id v2, Id v3, Id v4, float x, float y,
        float z);
// Special case, add a dangling edge (Simple Make Edge Vertex)
int Smev(Id s, Id f1, Id v1, Id v4, float x, float y, float z);
// Inverse (Kill Edge Vertex)
int Kev(Id s, Id f, Id v1, Id v2);

// Face splitting operator (Make Edge Face)
int Mef(Id s, Id f1, Id f2, Id v1, Id v2, Id v3, Id v4);
// Special case, if v1 and v3 are known to appear just once in the face (Simple
// Make Edge Face)
int Smef(Id s, Id f1, Id v1, Id v3, Id f2);
// Inverse (Kill Edge Face)
int Kef(Id s, Id f, Id v1, Id v2);
// Ring movement addendum (Ring Move)
int Ringmv(bdSolid* s, Id f1, Id f2, Id v1, Id v2, int inout);

// Loop splitting operator (Kill Edge Make Ring)
int Kemr(Id s, Id f, Id v1, Id v2);
// Inverse (Make Edge Kill Ring)
int Mekr(Id s, Id f, Id v1, Id v2, Id v3, Id v4);
// Special case (Simple Make Edge Kill Ring)
int Smekr(Id s, Id f, Id v1, Id v3);

// Face merging by creating an inner loop (Kill Face Make Ring Hole)
int Kfmrh(Id s, Id f1, Id f2);
// Inverse (Make Face Kill Ring Hole)
int Mfkrh(Id s, Id f1, Id v1, Id v2, Id f2);
