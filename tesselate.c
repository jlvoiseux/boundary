#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "tesselate.h"

#include <malloc.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>

#include "defines.h"

bdMesh* TessellateSolid(const bdSolid* solid, const bdTessellationParams* params) {
  NULL_CHECK_RET(solid, NULL);
  NULL_CHECK_RET(params, NULL);

  bdMesh* mesh = (bdMesh*)malloc(sizeof(bdMesh));
  mesh->vertex_capacity = 1024;
  mesh->triangle_capacity = 1024;
  mesh->vertices = (bdMeshVertex*)malloc(sizeof(bdMeshVertex) * mesh->vertex_capacity);
  mesh->triangles = (bdTriangle*)malloc(sizeof(bdTriangle) * mesh->triangle_capacity);
  mesh->vertex_count = 0;
  mesh->triangle_count = 0;

  bdFace* f = solid->sfaces;
  while (f) {
    TessellateFace(mesh, f, params);
    f = f->nextf;
  }

  return mesh;
}

void TessellateFace(bdMesh* mesh, const bdFace* face, const bdTessellationParams* params) {
  NULL_CHECK(mesh);
  NULL_CHECK(face);

  bdLoop* l = face->floops;
  while (l) {
    Vector3* points = NULL;
    int point_count = 0;

    ExtractLoopPoints(l, &points, &point_count);

    if (point_count == 4) {
      AddTriangle(mesh, points[0], points[1], points[2]);
      AddTriangle(mesh, points[0], points[2], points[3]);
    }
    else if (point_count > 4) {
      Vector3* concave_points = NULL;
      int concave_count = 0;

      FindConcaveVertices(points, point_count, &concave_points, &concave_count);

      if (concave_count > 0) {
        HandleConcavePolygon(mesh, points, point_count, concave_points, concave_count);
      } else {
        TriangulateConvexPolygon(mesh, points, point_count);
      }

      free(concave_points);
    } else if (point_count == 3) {
      AddTriangle(mesh, points[0], points[1], points[2]);
    }

    free(points);
    l = l->nextl;
  }
}

void ExtractLoopPoints(const bdLoop* loop, Vector3** points, int* point_count) {
  NULL_CHECK(loop);
  NULL_CHECK(points);
  NULL_CHECK(point_count);

  bdHalfEdge* start = loop->ledg;
  bdHalfEdge* current = start;
  int count = 0;

  do {
    count++;
    current = current->nxt;
  } while (current != start);

  *points = (Vector3*)malloc(sizeof(Vector3) * count);
  *point_count = count;

  current = start;
  int i = 0;
  do {
    (*points)[i].x = current->vtx->vcoord[0];
    (*points)[i].y = current->vtx->vcoord[1];
    (*points)[i].z = current->vtx->vcoord[2];
    i++;
    current = current->nxt;
  } while (current != start);
}

Vector3 CalculateNormal(Vector3 v1, Vector3 v2, Vector3 v3) {
  Vector3 edge1 = Vector3Subtract(v2, v1);
  Vector3 edge2 = Vector3Subtract(v3, v1);
  return Vector3Normalize(Vector3CrossProduct(edge1, edge2));
}

float CalculateAngle(Vector3 prev, Vector3 current, Vector3 next) {
  Vector3 v1 = Vector3Subtract(prev, current);
  Vector3 v2 = Vector3Subtract(next, current);

  float dot = Vector3DotProduct(Vector3Normalize(v1), Vector3Normalize(v2));
  return acosf(Clamp(dot, -1.0f, 1.0f)) * RAD2DEG;
}

void FindConcaveVertices(const Vector3* points, int point_count,
                         Vector3** concave_points, int* concave_count) {
  NULL_CHECK(points);
  NULL_CHECK(concave_points);
  NULL_CHECK(concave_count);

  Vector3* result = (Vector3*)malloc(sizeof(Vector3) * point_count);
  int count = 0;

  Vector3 normal = CalculateNormal(points[0], points[1], points[2]);

  for (int i = 0; i < point_count; i++) {
    Vector3 prev = points[(i - 1 + point_count) % point_count];
    Vector3 current = points[i];
    Vector3 next = points[(i + 1) % point_count];

    Vector3 edge1 = Vector3Subtract(prev, current);
    Vector3 edge2 = Vector3Subtract(next, current);

    Vector3 cross = Vector3CrossProduct(edge1, edge2);
    float dot = Vector3DotProduct(cross, normal);

    if (dot < 0) {
      result[count++] = current;
    }
  }

  *concave_points = result;
  *concave_count = count;
}

bool IsPointVisible(Vector3 from, Vector3 to, const Vector3* points, int point_count) {
  Vector3 direction = Vector3Subtract(to, from);
  float distance = Vector3Length(direction);
  direction = Vector3Scale(direction, 1.0f / distance);

  for (int i = 0; i < point_count; i++) {
    Vector3 p1 = points[i];
    Vector3 p2 = points[(i + 1) % point_count];

    Vector3 v1 = Vector3Subtract(p1, from);
    Vector3 v2 = Vector3Subtract(p2, from);

    Vector3 normal = Vector3CrossProduct(v1, v2);
    if (Vector3Length(normal) < EPS) continue;

    float t = Vector3DotProduct(Vector3Subtract(p1, from), normal) /
              Vector3DotProduct(direction, normal);

    if (t > EPS && t < distance - EPS) {
      Vector3 intersection = Vector3Add(from, Vector3Scale(direction, t));
      if (IsPointOnLineSegment(intersection, p1, p2)) {
        return false;
      }
    }
  }

  return true;
}

void FindVisibleVertices(const Vector3* points, int point_count, int from_idx,
                         Vector3** visible, int* visible_count) {
  NULL_CHECK(points);
  NULL_CHECK(visible);
  NULL_CHECK(visible_count);

  Vector3* result = (Vector3*)malloc(sizeof(Vector3) * point_count);
  int count = 0;

  Vector3 from = points[from_idx];

  for (int i = 0; i < point_count; i++) {
    if (i == from_idx) continue;

    if (IsPointVisible(from, points[i], points, point_count)) {
      result[count++] = points[i];
    }
  }

  *visible = result;
  *visible_count = count;
}

void HandleConcavePolygon(bdMesh* mesh, const Vector3* points, int point_count,
                          const Vector3* concave_points, int concave_count) {
  for (int i = 0; i < concave_count; i++) {
    int concave_idx = 0;
    for (int j = 0; j < point_count; j++) {
      if (Vector3Equals(points[j], concave_points[i])) {
        concave_idx = j;
        break;
      }
    }

    Vector3* visible_vertices = NULL;
    int visible_count = 0;

    FindVisibleVertices(points, point_count, concave_idx, &visible_vertices, &visible_count);

    if (visible_count > 0) {
      int best_connection = FindBestConnection(concave_points[i], visible_vertices, visible_count);
      AddTriangle(mesh, concave_points[i],
                  visible_vertices[best_connection],
                  visible_vertices[(best_connection + 1) % visible_count]);
    }

    free(visible_vertices);
  }
}

void TriangulateConvexPolygon(bdMesh* mesh, const Vector3* points, int point_count) {
  NULL_CHECK(mesh);
  NULL_CHECK(points);

  if (point_count < 3) return;

  for (int i = 1; i < point_count - 1; i++) {
    AddTriangle(mesh, points[0], points[i], points[i + 1]);
  }
}

int FindBestConnection(Vector3 from, const Vector3* to_points, int point_count) {
  if (point_count == 0) return -1;
  if (point_count == 1) return 0;

  float best_angle = 0;
  int best_idx = 0;

  for (int i = 0; i < point_count; i++) {
    Vector3 prev = to_points[(i - 1 + point_count) % point_count];
    Vector3 current = to_points[i];
    Vector3 next = to_points[(i + 1) % point_count];

    float angle = CalculateAngle(prev, current, next);
    if (angle > best_angle) {
      best_angle = angle;
      best_idx = i;
    }
  }

  return best_idx;
}

bool IsPointOnLineSegment(Vector3 point, Vector3 a, Vector3 b) {
  Vector3 ab = Vector3Subtract(b, a);
  Vector3 ap = Vector3Subtract(point, a);

  float dot = Vector3DotProduct(ap, ab);
  float len_sq = Vector3DotProduct(ab, ab);

  if (dot < 0 || dot > len_sq) return false;

  float d = Vector3Length(Vector3CrossProduct(ap, ab)) / Vector3Length(ab);
  return d < EPS;
}

int AddVertex(bdMesh* mesh, Vector3 position) {
  if (mesh->vertex_count >= mesh->vertex_capacity) {
    mesh->vertex_capacity *= 2;
    mesh->vertices = (bdMeshVertex*)realloc(mesh->vertices,
                                          sizeof(bdMeshVertex) * mesh->vertex_capacity);
  }

  int idx = mesh->vertex_count++;
  mesh->vertices[idx].position = position;
  return idx;
}

void AddTriangle(bdMesh* mesh, Vector3 v1, Vector3 v2, Vector3 v3) {
  if (mesh->triangle_count >= mesh->triangle_capacity) {
    mesh->triangle_capacity *= 2;
    mesh->triangles = (bdTriangle*)realloc(mesh->triangles,
                                           sizeof(bdTriangle) * mesh->triangle_capacity);
  }

  int idx = mesh->triangle_count++;
  mesh->triangles[idx].v1 = AddVertex(mesh, v1);
  mesh->triangles[idx].v2 = AddVertex(mesh, v2);
  mesh->triangles[idx].v3 = AddVertex(mesh, v3);
}

Mesh ConvertToRaylibMesh(const bdMesh* mesh) {
  NULL_CHECK_RET(mesh, (Mesh){0});

  Mesh raylibMesh = {0};
  raylibMesh.vertexCount = mesh->vertex_count * 3;  // Multiply by 3 as each vertex needs unique barycentric coords
  raylibMesh.triangleCount = mesh->triangle_count;

  raylibMesh.vertices = RL_MALLOC(raylibMesh.vertexCount * 3 * sizeof(float));
  raylibMesh.normals = RL_MALLOC(raylibMesh.vertexCount * 3 * sizeof(float));
  raylibMesh.texcoords = RL_MALLOC(raylibMesh.vertexCount * 2 * sizeof(float));
  raylibMesh.indices = RL_MALLOC(mesh->triangle_count * 3 * sizeof(unsigned short));

  for (int i = 0; i < mesh->triangle_count; i++) {
    int base = i * 9;
    int texbase = i * 6;

    raylibMesh.vertices[base] = mesh->vertices[mesh->triangles[i].v1].position.x;
    raylibMesh.vertices[base + 1] = mesh->vertices[mesh->triangles[i].v1].position.y;
    raylibMesh.vertices[base + 2] = mesh->vertices[mesh->triangles[i].v1].position.z;
    raylibMesh.texcoords[texbase] = 1.0f;
    raylibMesh.texcoords[texbase + 1] = 0.0f;

    raylibMesh.vertices[base + 3] = mesh->vertices[mesh->triangles[i].v2].position.x;
    raylibMesh.vertices[base + 4] = mesh->vertices[mesh->triangles[i].v2].position.y;
    raylibMesh.vertices[base + 5] = mesh->vertices[mesh->triangles[i].v2].position.z;
    raylibMesh.texcoords[texbase + 2] = 0.0f;
    raylibMesh.texcoords[texbase + 3] = 1.0f;

    raylibMesh.vertices[base + 6] = mesh->vertices[mesh->triangles[i].v3].position.x;
    raylibMesh.vertices[base + 7] = mesh->vertices[mesh->triangles[i].v3].position.y;
    raylibMesh.vertices[base + 8] = mesh->vertices[mesh->triangles[i].v3].position.z;
    raylibMesh.texcoords[texbase + 4] = 0.0f;
    raylibMesh.texcoords[texbase + 5] = 0.0f;

    raylibMesh.indices[i * 3] = i * 3;
    raylibMesh.indices[i * 3 + 1] = i * 3 + 1;
    raylibMesh.indices[i * 3 + 2] = i * 3 + 2;
  }

  return raylibMesh;
}

void FreeMesh(bdMesh* mesh) {
  if (mesh) {
    free(mesh->vertices);
    free(mesh->triangles);
    free(mesh);
  }
}

int ExportMeshToStl(const bdMesh* mesh, const char* filepath) {
  NULL_CHECK_RET(mesh, ERROR)
  NULL_CHECK_RET(filepath, ERROR)

  FILE* file = fopen(filepath, "wb");
  NULL_CHECK_RET(file, ERROR);

  char header[80] = {0};
  uint32_t triangle_count = mesh->triangle_count;

  fwrite(header, sizeof(header), 1, file);
  fwrite(&triangle_count, sizeof(uint32_t), 1, file);

  for (int i = 0; i < mesh->triangle_count; i++) {
    Vector3 v1 = mesh->vertices[mesh->triangles[i].v1].position;
    Vector3 v2 = mesh->vertices[mesh->triangles[i].v2].position;
    Vector3 v3 = mesh->vertices[mesh->triangles[i].v3].position;

    Vector3 normal = CalculateNormal(v1, v2, v3);

    float buffer[12] = {
        normal.x, normal.y, normal.z,
        v1.x, v1.y, v1.z,
        v2.x, v2.y, v2.z,
        v3.x, v3.y, v3.z
    };

    uint16_t attribute_byte_count = 0;

    fwrite(buffer, sizeof(float), 12, file);
    fwrite(&attribute_byte_count, sizeof(uint16_t), 1, file);
  }

  fclose(file);
  return SUCCESS;
}