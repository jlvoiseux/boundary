#ifdef __cplusplus
#error "Must be compiled with a C compiler"
#endif

#include "gui.h"
#include "render.h"

static RenderTexture2D g_viewport_texture;
static Camera3D g_camera = {0};

static void SetupScene(void) {
  g_camera.position = (Vector3){10.0f, 10.0f, 10.0f};
  g_camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  g_camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  g_camera.fovy = 45.0f;
  g_camera.projection = CAMERA_PERSPECTIVE;
}

void GuiSetup(bool dark) {
  InitWindow(1920, 1080, "Bound");
  SetTargetFPS(60);

  g_viewport_texture = LoadRenderTexture(1920, 1080);

  igCreateContext(NULL);
  ImGuiIO *ioptr = igGetIO();
  ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  if (dark) {
    igStyleColorsDark(NULL);
  } else {
    igStyleColorsLight(NULL);
  }

  ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui_ImplRaylib_Init();

  ImFontAtlas_AddFontDefault(ioptr->Fonts, NULL);
  rligSetupFontAwesome();

  ImGui_ImplRaylib_BuildFontAtlas();

  SetupScene();
}

static void RenderViewport(void) {
  BeginTextureMode(g_viewport_texture);
  ClearBackground(RAYWHITE);

  BeginMode3D(g_camera);
  DrawGrid(10, 1.0f);

  bdSolid* s = firsts;
  while (s) {
    RenderSolid(s);
    s = s->nexts;
  }

  EndMode3D();
  EndTextureMode();
}

void GuiBegin(void) {
  if (igIsItemHovered(ImGuiHoveredFlags_None)) {
    UpdateCamera(&g_camera, CAMERA_ORBITAL);
  }
  RenderViewport();

  ImGui_ImplRaylib_ProcessEvents();
  ImGui_ImplRaylib_NewFrame();
  igNewFrame();

  bool dockspace_open = true;

  ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

  ImGuiViewport *viewport = igGetMainViewport();
  igSetNextWindowPos(viewport->Pos, ImGuiCond_Always, (ImVec2){0, 0});
  igSetNextWindowSize(viewport->Size, ImGuiCond_Always);
  igSetNextWindowViewport(viewport->ID);
  igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 0});
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                  ImGuiWindowFlags_NoBringToFrontOnFocus |
                  ImGuiWindowFlags_NoNavFocus;

  igBegin("DockSpace", &dockspace_open, window_flags);
  igPopStyleVar(1);
  igDockSpace(igGetID_Str("MyDockSpace"), (ImVec2){0, 0}, dockspace_flags,
              NULL);

  igBegin("Viewport", NULL, ImGuiWindowFlags_NoScrollbar);
  ImVec2 viewport_size;
  igGetContentRegionAvail(&viewport_size);
  if (viewport_size.x != (float)g_viewport_texture.texture.width ||
      viewport_size.y != (float)g_viewport_texture.texture.height) {
    UnloadRenderTexture(g_viewport_texture);
    g_viewport_texture =
        LoadRenderTexture((int)viewport_size.x, (int)viewport_size.y);
  }
  rligImageRenderTexture(&g_viewport_texture);
  igEnd();
}

void GuiEnd(void) {
  igEnd();  // dockspace

  igRender();

  BeginDrawing();
  ClearBackground(RAYWHITE);
  ImGui_ImplRaylib_RenderDrawData(igGetDrawData());
  EndDrawing();
}

void GuiShutdown(void) {
  UnloadRenderTexture(g_viewport_texture);
  ImGui_ImplRaylib_Shutdown();
  igDestroyContext(NULL);
  CloseWindow();
}