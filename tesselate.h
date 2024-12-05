#pragma once

#include <raylib.h>
#include "prim.h"

typedef struct {
  Vector3 position;
  Vector3 normal;
} bdMeshVertex;

typedef struct {
  int v1;
  int v2;
  int v3;
  Vector3 face_normal;
} bdTriangle;

typedef struct {
  bdMeshVertex* vertices;
  int vertex_count;
  bdTriangle* triangles;
  int triangle_count;
  int vertex_capacity;
  int triangle_capacity;
} bdMesh;

typedef struct {
  float chord_height;
  float max_edge_length;
  float min_angle;
} bdTessellationParams;

bdMesh* TessellateSolid(const bdSolid* solid, const bdTessellationParams* params);
Mesh ConvertToRaylibMesh(const bdMesh* mesh);
void FreeMesh(bdMesh* mesh);
int ExportMeshToStl(const bdMesh* mesh, const char* filepath);
