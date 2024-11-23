#pragma once

#include "linalg.h"

typedef int Id;

typedef struct bdSolid bdSolid;
typedef struct bdFace bdFace;
typedef struct bdLoop bdLoop;
typedef struct bdHalfEdge bdHalfEdge;
typedef struct bdVertex bdVertex;
typedef struct bdEdge bdEdge;
typedef union bdNode bdNode;

struct bdSolid {
  Id solidno;
  bdFace* sfaces;
  bdEdge* sedges;
  bdVertex* sverts;
  bdSolid* nexts;
  bdSolid* prevs;
};

struct bdFace {
  Id faceno;
  bdSolid* fsolid;
  bdLoop* flout;
  bdLoop* floops;
  bdVector feq;
  bdFace* nextf;
  bdFace* prevf;
};

struct bdLoop {
  bdHalfEdge* ledg;
  bdFace* lface;
  bdLoop* nextl;
  bdLoop* prevl;
};

struct bdEdge {
  bdHalfEdge* he1;
  bdHalfEdge* he2;
  bdEdge* nexte;
  bdEdge* preve;
};

struct bdHalfEdge {
  bdEdge* edg;
  bdVertex* vtx;
  bdLoop* wloop;
  bdHalfEdge* nxt;
  bdHalfEdge* prv;
};

struct bdVertex {
  Id vertexno;
  bdHalfEdge* vedge;
  bdVector vcoord;
  bdVertex* nextv;
  bdVertex* prevv;
};

union bdNode {
  bdSolid s;
  bdFace f;
  bdLoop l;
  bdHalfEdge h;
  bdVertex v;
  bdEdge e;
};

bdNode* CreateNode(int what, bdNode* where);
void DeleteNode(int what, bdNode* which, bdNode* where);

void Append(int what, bdNode* which, bdNode* where);
void Remove(int what, bdNode* which, bdNode* where);

bdHalfEdge* AppendHalfEdge(bdEdge* e, bdVertex* v, bdHalfEdge* where, int sign);
bdHalfEdge* RemoveHalfEdge(bdHalfEdge* he);

extern bdSolid* firsts;
extern Id maxs;
extern Id maxf;
extern Id maxv;
extern double EPS;
extern double BIGEPS;