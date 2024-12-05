#pragma once

#pragma once

#include <raylib.h>
#include "prim.h"

typedef struct {
  Vector3 position;
  Vector3 normal;
} bdMeshVertex;

typedef struct {
  Vector3 position;
  float angle;
  int index;
} bdOrderedVertex;

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

void TessellateFace(bdMesh* mesh, const bdFace* face, const bdTessellationParams* params);
void HandleConcavePolygon(bdMesh* mesh, const Vector3* points, int point_count,
                          const Vector3* concave_points, int concave_count, Vector3 normal);
void ExtractLoopPoints(const bdLoop* loop, Vector3** points, int* point_count);
void FindConcaveVertices(const Vector3* points, int point_count,
                                Vector3** concave_points, int* concave_count);
void FindVisibleVertices(const Vector3* points, int point_count, int from_idx,
                                Vector3** visible, int* visible_count);
void TriangulateConvexPolygon(bdMesh* mesh, const Vector3* points, int point_count, Vector3 normal);
int FindBestConnection(Vector3 from, const Vector3* to_points, int point_count);
int AddVertex(bdMesh* mesh, Vector3 position);
void AddTriangle(bdMesh* mesh, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 normal);
Vector3 CalculateNormal(Vector3 v1, Vector3 v2, Vector3 v3);
float CalculateAngle(Vector3 prev, Vector3 current, Vector3 next);
bool IsPointVisible(Vector3 from, Vector3 to, const Vector3* points, int point_count);
bool IsPointOnLineSegment(Vector3 point, Vector3 a, Vector3 b);
bdMesh* TessellateSolid(const bdSolid* solid, const bdTessellationParams* params);
Mesh ConvertToRaylibMesh(const bdMesh* mesh);
void FreeMesh(bdMesh* mesh);

int ExportMeshToStl(const bdMesh* mesh, const char* filepath);
