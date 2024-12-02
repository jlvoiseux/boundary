#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "render.h"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include "tesselate.h"

void LoadShaders(Material* material) {
  const char* vert_path = "shaders/wireframe.vert";
  const char* frag_path = "shaders/wireframe.frag";

  Shader shader = LoadShader(vert_path, frag_path);
  if (shader.id == 0) {
    TraceLog(LOG_ERROR, "Failed to load wireframe shader");
    return;
  }

  rlSetCullFace(0);

  int wire_width_loc = GetShaderLocation(shader, "wireWidth");
  int wire_color_loc = GetShaderLocation(shader, "wireColor");
  int face_color_loc = GetShaderLocation(shader, "faceColor");

  float wire_color[3] = {0.0f, 0.7f, 1.0f};
  float face_color[3] = {0.2f, 0.2f, 0.2f};
  float wire_width = 1.0f;

  SetShaderValue(shader, wire_width_loc, &wire_width, SHADER_UNIFORM_FLOAT);
  SetShaderValue(shader, wire_color_loc, wire_color, SHADER_UNIFORM_VEC3);
  SetShaderValue(shader, face_color_loc, face_color, SHADER_UNIFORM_VEC3);

  material->shader = shader;
  material->maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  TraceLog(LOG_INFO, "Shader loaded successfully: ID %u", shader.id);
}

void RenderGizmo(const Camera3D* mainCamera, float width, float height) {
  float gizmoSize = 40.0f;
  Vector2 position = {width - gizmoSize - 20.0f, gizmoSize + 20.0f};

  Matrix viewMatrix = MatrixLookAt(mainCamera->position, mainCamera->target, mainCamera->up);

  Vector3 xAxis3D = Vector3Transform((Vector3){1.0f, 0.0f, 0.0f}, viewMatrix);
  Vector3 yAxis3D = Vector3Transform((Vector3){0.0f, 1.0f, 0.0f}, viewMatrix);
  Vector3 zAxis3D = Vector3Transform((Vector3){0.0f, 0.0f, 1.0f}, viewMatrix);

  Vector2 xAxis = {xAxis3D.x * gizmoSize, -xAxis3D.y * gizmoSize};
  Vector2 yAxis = {yAxis3D.x * gizmoSize, -yAxis3D.y * gizmoSize};
  Vector2 zAxis = {zAxis3D.x * gizmoSize, -zAxis3D.y * gizmoSize};

  DrawLineV(position, Vector2Add(position, xAxis), RED);
  DrawLineV(position, Vector2Add(position, yAxis), GREEN);
  DrawLineV(position, Vector2Add(position, zAxis), BLUE);

  DrawCircleV(Vector2Add(position, xAxis), 3.0f, RED);
  DrawCircleV(Vector2Add(position, yAxis), 3.0f, GREEN);
  DrawCircleV(Vector2Add(position, zAxis), 3.0f, BLUE);

  DrawCircleV(position, 4.0f, DARKGRAY);

  Vector2 labelOffset = {5.0f, -5.0f};
  DrawText("X", position.x + xAxis.x + labelOffset.x, position.y + xAxis.y + labelOffset.y, 10, RED);
  DrawText("Y", position.x + yAxis.x + labelOffset.x, position.y + yAxis.y + labelOffset.y, 10, GREEN);
  DrawText("Z", position.x + zAxis.x + labelOffset.x, position.y + zAxis.y + labelOffset.y, 10, BLUE);
}

void UpdateRenderObject(bdRenderObject* obj)
{
  if (obj->mesh_dirty) {
    if (obj->mesh.vboId && obj->mesh.vboId[0] > 0) {
      UnloadMesh(obj->mesh);
    }

    bdTessellationParams params = {
        .chord_height = 0.1f,
        .max_edge_length = 1.0f,
        .min_angle = 15.0f
    };

    bdMesh* temp_mesh = TessellateSolid(obj->solid, &params);
    obj->mesh = ConvertToRaylibMesh(temp_mesh);

    // Now explicitly upload to GPU
    UploadMesh(&obj->mesh, true);  // Use dynamic if you plan to update buffers frequently

    FreeMesh(temp_mesh);
    obj->mesh_dirty = false;
  }
}

void RenderMainView(const Camera3D* camera, const bdRenderObject* obj) {
  DrawGrid(50, 1.0f);
  DrawMesh(obj->mesh, obj->mat, obj->tr);
}

