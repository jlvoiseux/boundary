#pragma once

#include "prim.h"

typedef enum {
  CMD_MVFS,
  CMD_KVFS,
  CMD_MEV,
  CMD_SMEV,
  CMD_KEV,
  CMD_MEF,
  CMD_SMEF,
  CMD_KEF,
  CMD_KEMR,
  CMD_MEKR,
  CMD_SMEKR,
  CMD_KFMRH,
  CMD_MFKRH,
  CMD_SWEEP,
  CMD_RSWEEP,
  CMD_ARC,
  CMD_CIRCLE,
  CMD_ERROR
} bdCommandType;

typedef struct {
  bdCommandType type;
  float args[10];
  int arg_count;
  char error_msg[256];
} bdCommand;

bdCommand ParseCommand(const char* line);
bdSolid* ExecuteCommands(const char* script, char* error_msg, size_t error_size);