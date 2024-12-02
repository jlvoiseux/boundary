#pragma once

#include <raylib.h>

#include "prim.h"
#include "rlimgui/imgui_impl_raylib.h"
#include "rlimgui/rlcimgui.h"

typedef enum {
  CAMERA_VIEW_FRONT,
  CAMERA_VIEW_BACK,
  CAMERA_VIEW_RIGHT,
  CAMERA_VIEW_LEFT,
  CAMERA_VIEW_TOP,
  CAMERA_VIEW_BOTTOM
} CameraView;

typedef struct {
  Camera3D camera;
  Vector3 target;
  bool isPerspective;
  CameraView currentView;
  Vector2 previousMousePosition;
  float orbitRadius;
} bdCameraSystem;

typedef struct {
  char message[256];
  float time_remaining;
  bool active;
  bool is_error;
} bdToast;

void GuiSetup(bool dark);
void GuiBegin(void);
void GuiEnd(void);
void GuiShutdown(void);

void InitCamera();
void HandleViewSwitch();
void HandleCameraMovement();

void RenderViewport();

void DrawControlsWindow();
void DrawViewportWindow();

void ShowToast(const char* message, bool is_error);
void UpdateToasts(void);
