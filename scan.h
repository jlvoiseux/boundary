#pragma once

#include "prim.h"

void ListSolid(bdSolid* s);
void ListNeighbors(bdVertex* v);

bdSolid* GetSolid(Id sn);
bdFace* GetFace(bdSolid* s, Id fn);
bdHalfEdge* GetHalfEdge(bdFace* f, Id vn1, Id vn2);

void GetMaxNames(Id sn);

int Match(bdHalfEdge* h1, bdHalfEdge* h2);