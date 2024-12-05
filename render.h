#pragma once

#include <raylib.h>
#include "prim.h"

typedef enum {
  RENDER_MODE_TESSELLATED,
  RENDER_MODE_BREP
} bdRenderMode;

typedef struct {
  bdSolid* solid;
  Mesh mesh;
  Material mat;
  Matrix tr;
  bool mesh_dirty;
  bdRenderMode mode;
  char script[4096];
} bdRenderObject;

void LoadShaders(Material* material);
void UpdateRenderObject(bdRenderObject* obj);

void RenderMainView(const Camera3D* camera, const bdRenderObject* obj);
void RenderGizmo(const Camera3D* mainCamera, float width, float height);