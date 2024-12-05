#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "tesselate.h"
#include <malloc.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"

    typedef struct {
  Vector3 position;
  int index;
  int isEar;
} bdVertexNode;

static float SignedArea3D(Vector3 a, Vector3 b, Vector3 c, Vector3 normal) {
  Vector3 ab = Vector3Subtract(b, a);
  Vector3 ac = Vector3Subtract(c, a);
  Vector3 cross = Vector3CrossProduct(ab, ac);
  return Vector3DotProduct(cross, normal);
}

static int IsPointInTriangle3D(Vector3 p, Vector3 a, Vector3 b, Vector3 c, Vector3 normal) {
  float area = SignedArea3D(a, b, c, normal);
  float s1 = SignedArea3D(p, a, b, normal);
  float s2 = SignedArea3D(p, b, c, normal);
  float s3 = SignedArea3D(p, c, a, normal);

  if (ABS(area) < EPS) return 0;
  float r1 = s1 / area;
  float r2 = s2 / area;
  float r3 = s3 / area;

  return (r1 >= -EPS && r2 >= -EPS && r3 >= -EPS);
}

static int IsEar(bdVertexNode* vertices, int count, int i, Vector3 normal) {
  if (count < 4) return 1;

  int prev = (i - 1 + count) % count;
  int next = (i + 1) % count;
  Vector3 a = vertices[prev].position;
  Vector3 b = vertices[i].position;
  Vector3 c = vertices[next].position;

  if (SignedArea3D(a, b, c, normal) <= 0) return 0;

  for (int j = 0; j < count; j++) {
    if (j == prev || j == i || j == next) continue;
    if (IsPointInTriangle3D(vertices[j].position, a, b, c, normal)) return 0;
  }

  return 1;
}

static void UpdateVertexType(bdVertexNode* vertices, int count, int i, Vector3 normal) {
  vertices[i].isEar = IsEar(vertices, count, i, normal);
}

static void ExtractLoopPoints(const bdLoop* loop, Vector3** points, int* point_count) {
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

  *points = malloc(sizeof(Vector3) * count);
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

static Vector3 CalculateNormal(Vector3 v1, Vector3 v2, Vector3 v3) {
  Vector3 edge1 = Vector3Subtract(v2, v1);
  Vector3 edge2 = Vector3Subtract(v3, v1);
  return Vector3Normalize(Vector3CrossProduct(edge1, edge2));
}

static int AddVertex(bdMesh* mesh, Vector3 position) {
  if (mesh->vertex_count >= mesh->vertex_capacity) {
    mesh->vertex_capacity *= 2;
    mesh->vertices = realloc(mesh->vertices, sizeof(bdMeshVertex) * mesh->vertex_capacity);
  }

  int idx = mesh->vertex_count++;
  mesh->vertices[idx].position = position;
  return idx;
}

static void AddTriangle(bdMesh* mesh, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 normal) {
  if (mesh->triangle_count >= mesh->triangle_capacity) {
    mesh->triangle_capacity *= 2;
    mesh->triangles = realloc(mesh->triangles, sizeof(bdTriangle) * mesh->triangle_capacity);
  }

  int idx = mesh->triangle_count++;
  mesh->triangles[idx].v1 = AddVertex(mesh, v1);
  mesh->triangles[idx].v2 = AddVertex(mesh, v2);
  mesh->triangles[idx].v3 = AddVertex(mesh, v3);
  mesh->triangles[idx].face_normal = normal;
}

static void TriangulatePolygon(bdMesh* mesh, Vector3* points, int point_count, Vector3 normal) {
  if (point_count < 3) return;
  if (point_count == 3) {
    AddTriangle(mesh, points[0], points[1], points[2], normal);
    return;
  }

  bdVertexNode* vertices = malloc(sizeof(bdVertexNode) * point_count);
  int remaining = point_count;

  for (int i = 0; i < point_count; i++) {
    vertices[i].position = points[i];
    vertices[i].index = i;
  }

  for (int i = 0; i < point_count; i++) {
    UpdateVertexType(vertices, point_count, i, normal);
  }

  while (remaining > 3) {
    int earFound = 0;

    for (int i = 0; i < point_count; i++) {
      if (vertices[i].index == -1 || !vertices[i].isEar) continue;

      int prev = i;
      int curr = i;
      while (vertices[prev = ((prev - 1 + point_count) % point_count)].index == -1);
      while (vertices[curr = ((curr + 1) % point_count)].index == -1);

      AddTriangle(mesh, vertices[prev].position, vertices[i].position,
                  vertices[curr].position, normal);

      vertices[i].index = -1;
      remaining--;
      earFound = 1;

      if (remaining > 3) {
        UpdateVertexType(vertices, point_count, prev, normal);
        UpdateVertexType(vertices, point_count, curr, normal);
      }

      break;
    }

    if (!earFound) break;
  }

  if (remaining == 3) {
    int first = -1;
    for (int i = 0; i < point_count; i++) {
      if (vertices[i].index != -1) {
        if (first == -1) {
          first = i;
        } else if (vertices[(i + 1) % point_count].index != -1) {
          AddTriangle(mesh, vertices[first].position, vertices[i].position,
                      vertices[(i + 1) % point_count].position, normal);
          break;
        }
      }
    }
  }

  free(vertices);
}

static void TessellateFace(bdMesh* mesh, const bdFace* face, const bdTessellationParams* params) {
  NULL_CHECK(mesh);
  NULL_CHECK(face);

  bdLoop* l = face->floops;
  while (l) {
    Vector3* points = NULL;
    int point_count = 0;

    ExtractLoopPoints(l, &points, &point_count);

    if (point_count >= 3) {
      Vector3 normal = CalculateNormal(points[0], points[1], points[2]);
      TriangulatePolygon(mesh, points, point_count, normal);
    }

    free(points);
    l = l->nextl;
  }
}

bdMesh* TessellateSolid(const bdSolid* solid, const bdTessellationParams* params) {
  NULL_CHECK_RET(solid, NULL);
  NULL_CHECK_RET(params, NULL);

  bdMesh* mesh = malloc(sizeof(bdMesh));
  mesh->vertex_capacity = 1024;
  mesh->triangle_capacity = 1024;
  mesh->vertices = malloc(sizeof(bdMeshVertex) * mesh->vertex_capacity);
  mesh->triangles = malloc(sizeof(bdTriangle) * mesh->triangle_capacity);
  mesh->vertex_count = 0;
  mesh->triangle_count = 0;

  bdFace* f = solid->sfaces;
  while (f) {
    TessellateFace(mesh, f, params);
    f = f->nextf;
  }

  return mesh;
}


Mesh ConvertToRaylibMesh(const bdMesh* mesh) {
  NULL_CHECK_RET(mesh, (Mesh){0});

  Mesh raylibMesh = {0};
  raylibMesh.vertexCount = mesh->vertex_count * 3;
  raylibMesh.triangleCount = mesh->triangle_count;

  raylibMesh.vertices = RL_MALLOC(raylibMesh.vertexCount * 3 * sizeof(float));
  raylibMesh.normals = RL_MALLOC(raylibMesh.vertexCount * 3 * sizeof(float));
  raylibMesh.texcoords = RL_MALLOC(raylibMesh.vertexCount * 2 * sizeof(float));
  raylibMesh.indices = RL_MALLOC(mesh->triangle_count * 3 * sizeof(unsigned short));

  for (int i = 0; i < mesh->triangle_count; i++) {
    int base = i * 9;
    int texbase = i * 6;

    Vector3 normal = {
        mesh->triangles[i].face_normal.x,
        mesh->triangles[i].face_normal.y,
        mesh->triangles[i].face_normal.z
    };

    raylibMesh.vertices[base] = mesh->vertices[mesh->triangles[i].v1].position.x;
    raylibMesh.vertices[base + 1] = mesh->vertices[mesh->triangles[i].v1].position.y;
    raylibMesh.vertices[base + 2] = mesh->vertices[mesh->triangles[i].v1].position.z;
    raylibMesh.normals[base] = normal.x;
    raylibMesh.normals[base + 1] = normal.y;
    raylibMesh.normals[base + 2] = normal.z;
    raylibMesh.texcoords[texbase] = 1.0f;
    raylibMesh.texcoords[texbase + 1] = 0.0f;

    raylibMesh.vertices[base + 3] = mesh->vertices[mesh->triangles[i].v2].position.x;
    raylibMesh.vertices[base + 4] = mesh->vertices[mesh->triangles[i].v2].position.y;
    raylibMesh.vertices[base + 5] = mesh->vertices[mesh->triangles[i].v2].position.z;
    raylibMesh.normals[base + 3] = normal.x;
    raylibMesh.normals[base + 4] = normal.y;
    raylibMesh.normals[base + 5] = normal.z;
    raylibMesh.texcoords[texbase + 2] = 0.0f;
    raylibMesh.texcoords[texbase + 3] = 1.0f;

    raylibMesh.vertices[base + 6] = mesh->vertices[mesh->triangles[i].v3].position.x;
    raylibMesh.vertices[base + 7] = mesh->vertices[mesh->triangles[i].v3].position.y;
    raylibMesh.vertices[base + 8] = mesh->vertices[mesh->triangles[i].v3].position.z;
    raylibMesh.normals[base + 6] = normal.x;
    raylibMesh.normals[base + 7] = normal.y;
    raylibMesh.normals[base + 8] = normal.z;
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