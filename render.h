#pragma once

#include <raylib.h>
#include "prim.h"

typedef struct {
  bdSolid* solid;
  Mesh mesh;
  Material mat;
  Matrix tr;
  bool mesh_dirty;
} bdRenderObject;

void LoadShaders(Material* material);
void UpdateRenderObject(bdRenderObject* obj);

void RenderMainView(const Camera3D* camera, const bdRenderObject* obj);
void RenderGizmo(const Camera3D* mainCamera, float width, float height);