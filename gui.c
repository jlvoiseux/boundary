#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "gui.h"

#include <float.h>
#include <malloc.h>
#include <raymath.h>
#include <string.h>

#include "euler.h"
#include "parse.h"
#include "render.h"
#include "tesselate.h"

static RenderTexture2D g_viewport_texture;
static bdCameraSystem g_camera_system = {0};
static bdRenderObject g_render_object = {0};
static bdToast g_toast = {0};
static const float ZOOM_FACTOR = 0.1f;
static const float PAN_SPEED = 0.025f;
static const float ORBIT_SPEED = 0.01f;

static const bdPresetScript PRESETS[] = {
    {"Cube",
     "MVFS 1 1 1 0.0 0.0 0.0\n"
     "SMEV 1 1 1 2 2.0 0.0 0.0\n"
     "SMEV 1 1 2 3 2.0 2.0 0.0\n"
     "SMEV 1 1 3 4 0.0 2.0 0.0\n"
     "SMEF 1 1 1 4 2\n"
     "SWEEP 1 2 0.0 0.0 2.0\n"},

    {"Cylinder",
     "MVFS 1 1 1 2.0 0.0 0.0\n"
     "CIRCLE 1 0.0 0.0 2.0 0.0 32\n"
     "SWEEP 1 2 0.0 0.0 2.0\n"},

    {"Sphere",
     "MVFS 1 1 1 -2.0 0.0 0.0\n"
     "ARC 1 1 1 0.0 0.0 2.0 0.0 180.0 0.0 32\n"
     "RSWEEP 1 32 1.0 0.0 0.0\n"},

    {"Torus",
     "MVFS 1 1 1 0.5 1.5 0.0\n"
     "CIRCLE 1 0.0 1.5 0.5 0 32\n"
     "RSWEEP 1 32 1.0 0.0 0.0\n"}
};

static void ExecutePreset(const bdPresetScript* preset) {
  strcpy(g_render_object.script, preset->script);
  char error_msg[256] = {0};
  bdSolid* new_solid = ExecuteCommands(g_render_object.script, error_msg, sizeof(error_msg));
  if (new_solid) {
    if (g_render_object.solid) {
      Kvfs(g_render_object.solid);
    }
    g_render_object.solid = new_solid;
    g_render_object.mesh_dirty = true;
    ShowToast("Created successfully", false);
  } else {
    ShowToast(error_msg, true);
  }
}

void InitCamera() {
  g_camera_system.camera.position = (Vector3){10.0f, 10.0f, 10.0f};
  g_camera_system.target = (Vector3){0.0f, 0.0f, 0.0f};
  g_camera_system.camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  g_camera_system.camera.fovy = 25.0f;
  g_camera_system.camera.projection = CAMERA_PERSPECTIVE;
  g_camera_system.isPerspective = true;
  g_camera_system.currentView = CAMERA_VIEW_FRONT;
}

void InitRenderObject() {
  g_render_object.mat = LoadMaterialDefault();
  LoadShaders(&g_render_object.mat);
  g_render_object.tr = MatrixIdentity();
  g_render_object.mesh_dirty = true;
  g_render_object.mode = RENDER_MODE_BREP;
}

void GuiSetup(bool dark) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(1920, 1080, "Boundary");
  SetTargetFPS(60);

  g_viewport_texture = LoadRenderTexture(1920, 1080);

  igCreateContext(NULL);
  ImGuiIO* ioptr = igGetIO();
  ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  if (dark) {
    igStyleColorsDark(NULL);
  } else {
    igStyleColorsLight(NULL);
  }

  ImGui_ImplRaylib_Init();

  ImFontAtlas_AddFontDefault(ioptr->Fonts, NULL);
  rligSetupFontAwesome();

  ImGui_ImplRaylib_BuildFontAtlas();

  InitCamera();
  InitRenderObject();
}

void GuiBegin(void) {
  ImGui_ImplRaylib_ProcessEvents();
  ImGui_ImplRaylib_NewFrame();
  igNewFrame();

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  ImGuiViewport *viewport = igGetMainViewport();

  igSetNextWindowPos(viewport->Pos, ImGuiCond_Always, (ImVec2){0, 0});
  igSetNextWindowSize(viewport->Size, ImGuiCond_Always);
  igSetNextWindowViewport(viewport->ID);

  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                  ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 0});

  bool dockspace_open = true;
  igBegin("DockSpace Demo", &dockspace_open, window_flags);
  igPopStyleVar(1);

  ImGuiID dockspace_id = igGetID_Str("MyDockSpace");
  ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  igDockSpace(dockspace_id, (ImVec2){0, 0}, dockspace_flags, NULL);

  static bool first_time = true;
  if (first_time) {
    first_time = false;
    igDockBuilderRemoveNode(dockspace_id);
    igDockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_AutoHideTabBar);
    igDockBuilderSetNodeSize(dockspace_id, viewport->Size);

    ImGuiID dock_main_id = dockspace_id;
    ImGuiID dock_left = igDockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, NULL, &dock_main_id);

    igDockBuilderDockWindow("Controls", dock_left);
    igDockBuilderDockWindow("Viewport", dock_main_id);
    igDockBuilderFinish(dockspace_id);
  }

  DrawControlsWindow();
  DrawViewportWindow();
}

void GuiEnd(void) {
  igEnd();

  UpdateToasts();

  igRender();

  UpdateRenderObject(&g_render_object);

  RenderViewport();

  BeginDrawing();
  ClearBackground(WHITE);
  ImGui_ImplRaylib_RenderDrawData(igGetDrawData());
  EndDrawing();
}

void GuiShutdown(void) {
  UnloadRenderTexture(g_viewport_texture);
  ImGui_ImplRaylib_Shutdown();
  igDestroyContext(NULL);
  CloseWindow();
}

void HandleViewSwitch() {
  bool changed = true;
  Vector3 newPosition = g_camera_system.camera.position;
  float distance = g_camera_system.camera.fovy;

  if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
    g_camera_system.currentView = IsKeyDown(KEY_LEFT_CONTROL) ? CAMERA_VIEW_BACK : CAMERA_VIEW_FRONT;
    newPosition = IsKeyDown(KEY_LEFT_CONTROL) ?
                                              (Vector3){0.0f, 0.0f, -distance} : (Vector3){0.0f, 0.0f, distance};
  } else if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
    g_camera_system.currentView = IsKeyDown(KEY_LEFT_CONTROL) ? CAMERA_VIEW_LEFT : CAMERA_VIEW_RIGHT;
    newPosition = IsKeyDown(KEY_LEFT_CONTROL) ?
                                              (Vector3){-distance, 0.0f, 0.0f} : (Vector3){distance, 0.0f, 0.0f};
  } else if (IsKeyPressed(KEY_SEVEN) || IsKeyPressed(KEY_KP_7)) {
    g_camera_system.currentView = IsKeyDown(KEY_LEFT_CONTROL) ? CAMERA_VIEW_BOTTOM : CAMERA_VIEW_TOP;
    newPosition = IsKeyDown(KEY_LEFT_CONTROL) ?
                                              (Vector3){0.0f, -distance, 0.0f} : (Vector3){0.0f, distance, 0.0f};
  } else if (IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_KP_5)) {
    g_camera_system.isPerspective = !g_camera_system.isPerspective;
    if (!g_camera_system.isPerspective) {
      g_camera_system.camera.projection = CAMERA_ORTHOGRAPHIC;
    } else {
      g_camera_system.camera.projection = CAMERA_PERSPECTIVE;
    }
  } else {
    changed = false;
  }

  if (changed) {
    g_camera_system.camera.position = newPosition;
    g_camera_system.camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    if (g_camera_system.currentView == CAMERA_VIEW_TOP ||
        g_camera_system.currentView == CAMERA_VIEW_BOTTOM) {
      g_camera_system.camera.up = (Vector3){0.0f, 0.0f, -1.0f};
    }
  }
}

void HandleCameraMovement() {
  Vector2 mousePosition = GetMousePosition();
  Vector2 mouseDelta = {
      mousePosition.x - g_camera_system.previousMousePosition.x,
      -(mousePosition.y - g_camera_system.previousMousePosition.y)
  };

  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    g_camera_system.camera.fovy *= (1.0f - wheel * ZOOM_FACTOR);
    g_camera_system.camera.fovy = Clamp(g_camera_system.camera.fovy, 0.1f, 100.0f);

    if (g_camera_system.isPerspective) {
      Vector3 direction = Vector3Normalize(Vector3Subtract(g_camera_system.camera.position, g_camera_system.target));
      float distance = g_camera_system.camera.fovy;
      g_camera_system.camera.position = Vector3Add(g_camera_system.target, Vector3Scale(direction, distance));
    }
  }

  if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
      Vector3 right = Vector3Normalize(Vector3CrossProduct(
          Vector3Subtract(g_camera_system.camera.position, g_camera_system.target),
          g_camera_system.camera.up
          ));

      Vector3 panDelta = Vector3Add(
          Vector3Scale(right, mouseDelta.x * PAN_SPEED),
          Vector3Scale(g_camera_system.camera.up, -mouseDelta.y * PAN_SPEED)
      );

      g_camera_system.target = Vector3Add(g_camera_system.target, panDelta);
      g_camera_system.camera.position = Vector3Add(g_camera_system.camera.position, panDelta);
    }
    else {
      Vector3 direction = Vector3Subtract(g_camera_system.camera.position, g_camera_system.target);

      float rotationY = -mouseDelta.x * ORBIT_SPEED;
      float rotationX = -mouseDelta.y * ORBIT_SPEED;

      Matrix rotationMatrix = MatrixRotate(g_camera_system.camera.up, rotationY);
      direction = Vector3Transform(direction, rotationMatrix);

      Vector3 right = Vector3Normalize(Vector3CrossProduct(direction, g_camera_system.camera.up));
      rotationMatrix = MatrixRotate(right, rotationX);
      direction = Vector3Transform(direction, rotationMatrix);

      g_camera_system.camera.position = Vector3Add(g_camera_system.target, direction);
    }
  }

  g_camera_system.previousMousePosition = mousePosition;
  g_camera_system.camera.target = g_camera_system.target;
}

void DrawControlsWindow() {
  igBegin("Controls", NULL, ImGuiWindowFlags_None);

  if (igCollapsingHeader_TreeNodeFlags("Render Settings", ImGuiTreeNodeFlags_DefaultOpen)) {

    if (igRadioButton_Bool("Normals (B-Rep)", g_render_object.mode == RENDER_MODE_BREP)) {
      g_render_object.mode = RENDER_MODE_BREP;
    }
    if (igRadioButton_Bool("Tessellated", g_render_object.mode == RENDER_MODE_TESSELLATED)) {
      g_render_object.mode = RENDER_MODE_TESSELLATED;
    }
  }

  if (igCollapsingHeader_TreeNodeFlags("Script", ImGuiTreeNodeFlags_DefaultOpen)) {
    for (int i = 0; i < sizeof(PRESETS)/sizeof(PRESETS[0]); i++) {
      if (i > 0) igSameLine(0, 4);
      if (igButton(PRESETS[i].name, (ImVec2){60, 0})) {
        ExecutePreset(&PRESETS[i]);
      }
    }

    ImVec2 content_size;
    igGetContentRegionAvail(&content_size);
    igInputTextMultiline("##script", g_render_object.script,
                         sizeof(g_render_object.script),
                         (ImVec2){content_size.x, 200}, ImGuiInputTextFlags_AllowTabInput, NULL,
                         NULL);

    if (igButton("Execute", (ImVec2){120, 0})) {
      char error_msg[256] = {0};
      bdSolid* new_solid = ExecuteCommands(g_render_object.script, error_msg, sizeof(error_msg));

      if (new_solid) {
        if (g_render_object.solid) {
          Kvfs(g_render_object.solid);
        }
        g_render_object.solid = new_solid;
        g_render_object.mesh_dirty = true;
        ShowToast("Script executed successfully", false);
      } else {
        ShowToast(error_msg, true);
      }
    }
  }

  if (igCollapsingHeader_TreeNodeFlags("Export", ImGuiTreeNodeFlags_DefaultOpen)) {
    static char filepath[256] = {0};
    igInputText("STL Path", filepath, sizeof(filepath), ImGuiInputTextFlags_None, NULL, NULL);

    if (igButton("Export STL", (ImVec2){120, 0})) {
      if (strlen(filepath) > 0 && g_render_object.solid) {
        bdTessellationParams params = {
            .chord_height = 0.1f,
            .max_edge_length = 1.0f,
            .min_angle = 15.0f
        };

        bdMesh* temp_mesh = TessellateSolid(g_render_object.solid, &params);
        if (temp_mesh) {
          if (ExportMeshToStl(temp_mesh, filepath) == SUCCESS) {
            ShowToast("STL file exported successfully", false);
          } else {
            ShowToast("Failed to export STL file", true);
          }
          FreeMesh(temp_mesh);
        } else {
          ShowToast("Failed to tessellate mesh", true);
        }
      } else {
        ShowToast("Please enter a valid file path", true);
      }
    }
  }

  igEnd();
}

void RenderViewport() {
  BeginTextureMode(g_viewport_texture);
  ClearBackground(WHITE);

  BeginMode3D(g_camera_system.camera);
  RenderMainView(&g_camera_system.camera, &g_render_object);
  EndMode3D();

  RenderGizmo(&g_camera_system.camera, (float)g_viewport_texture.texture.width, (float)g_viewport_texture.texture.height);

  EndTextureMode();
}

void DrawViewportWindow() {
  igBegin("Viewport", NULL, ImGuiWindowFlags_NoScrollbar);

  ImVec2 viewport_size;
  igGetContentRegionAvail(&viewport_size);
  if (viewport_size.x != (float)g_viewport_texture.texture.width ||
      viewport_size.y != (float)g_viewport_texture.texture.height) {
    UnloadRenderTexture(g_viewport_texture);
    g_viewport_texture = LoadRenderTexture((int)viewport_size.x, (int)viewport_size.y);
  }

  ImVec2 window_pos;
  igGetWindowPos(&window_pos);

  ImVec2 cursor_pos;
  igGetCursorPos(&cursor_pos);

  rligImageRenderTexture(&g_viewport_texture);

  igSetCursorPos(cursor_pos);

  ImVec2 padding = {10.0f, 30.0f};
  igSetCursorPos(padding);

  ImVec4 bg_color = {0.0f, 0.0f, 0.0f, 0.7f};
  igPushStyleColor_Vec4(ImGuiCol_ChildBg, bg_color);

  igBeginChild_Str("CameraOverlay", (ImVec2){200, 80}, true,
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);

  igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4){1.0f, 1.0f, 1.0f, 1.0f});

  igText("Mode: %s", g_camera_system.isPerspective ? "Perspective" : "Orthographic");
  const char* viewNames[] = {"Front", "Back", "Right", "Left", "Top", "Bottom"};
  igText("View: %s", viewNames[g_camera_system.currentView]);
  igText("Pos: (%.1f, %.1f, %.1f)",
         g_camera_system.camera.position.x,
         g_camera_system.camera.position.y,
         g_camera_system.camera.position.z);

  igPopStyleColor(2);
  igEndChild();

  if (igIsWindowHovered(ImGuiHoveredFlags_None)) {
    HandleViewSwitch();
    HandleCameraMovement();
  }

  igEnd();
}

void ShowToast(const char* message, bool is_error) {
  strncpy(g_toast.message, message, sizeof(g_toast.message) - 1);
  g_toast.time_remaining = 2.0f;
  g_toast.active = true;
  g_toast.is_error = is_error;
}

void UpdateToasts(void) {
  if (!g_toast.active) {
    return;
  }

  g_toast.time_remaining -= GetFrameTime();
  if (g_toast.time_remaining <= 0) {
    g_toast.active = false;
    return;
  }

  float alpha = g_toast.time_remaining > 1.0f ? 1.0f : g_toast.time_remaining;
  ImVec4 bg_color = g_toast.is_error ?
                                     (ImVec4){0.8f, 0.2f, 0.2f, alpha * 0.9f} :
                                     (ImVec4){0.2f, 0.7f, 0.2f, alpha * 0.9f};
  ImVec4 text_color = {1.0f, 1.0f, 1.0f, alpha};

  ImGuiViewport* main_viewport = igGetMainViewport();
  ImVec2 viewport_size = main_viewport->Size;
  ImVec2 viewport_pos = main_viewport->Pos;

  igSetNextWindowBgAlpha(alpha * 0.9f);
  igPushStyleColor_Vec4(ImGuiCol_WindowBg, bg_color);
  igPushStyleColor_Vec4(ImGuiCol_Text, text_color);

  igSetNextWindowPos((ImVec2){
                         viewport_pos.x + viewport_size.x - 20.0f,
                         viewport_pos.y + viewport_size.y - 20.0f
                     }, ImGuiCond_Always, (ImVec2){1.0f, 1.0f});

  if (igBegin("##Toast", NULL,
              ImGuiWindowFlags_NoDecoration |
                  ImGuiWindowFlags_AlwaysAutoResize |
                  ImGuiWindowFlags_NoInputs |
                  ImGuiWindowFlags_NoNav)) {
    igText("%s", g_toast.message);
  }
  igEnd();

  igPopStyleColor(2);
}