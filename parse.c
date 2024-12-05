#include "parse.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "euler.h"
#include "scan.h"
#include "shape.h"
#include "transform.h"

static bdCommand CreateErrorCommand(const char* msg) {
  bdCommand cmd = {0};
  cmd.type = CMD_ERROR;
  strncpy(cmd.error_msg, msg, sizeof(cmd.error_msg) - 1);
  return cmd;
}

bdCommand ParseCommand(const char* line) {
  bdCommand cmd = {0};
  char command[32] = {0};
  char* ptr = (char*)line;
  int i = 0;

  while (*ptr && isspace(*ptr)) ptr++;
  if (!*ptr) return CreateErrorCommand("Empty line");

  while (*ptr && !isspace(*ptr) && i < sizeof(command)-1) {
    command[i++] = toupper(*ptr++);
  }
  command[i] = '\0';

  if (strcmp(command, "MVFS") == 0) {
    cmd.type = CMD_MVFS;
    cmd.arg_count = 6;
  } else if (strcmp(command, "KVFS") == 0) {
    cmd.type = CMD_KVFS;
    cmd.arg_count = 1;
  } else if (strcmp(command, "MEV") == 0) {
    cmd.type = CMD_MEV;
    cmd.arg_count = 10;
  } else if (strcmp(command, "SMEV") == 0) {
    cmd.type = CMD_SMEV;
    cmd.arg_count = 7;
  } else if (strcmp(command, "KEV") == 0) {
    cmd.type = CMD_KEV;
    cmd.arg_count = 4;
  } else if (strcmp(command, "MEF") == 0) {
    cmd.type = CMD_MEF;
    cmd.arg_count = 7;
  } else if (strcmp(command, "SMEF") == 0) {
    cmd.type = CMD_SMEF;
    cmd.arg_count = 5;
  } else if (strcmp(command, "KEF") == 0) {
    cmd.type = CMD_KEF;
    cmd.arg_count = 4;
  } else if (strcmp(command, "RINGMV") == 0) {
    cmd.type = CMD_RINGMV;
    cmd.arg_count = 5;
  } else if (strcmp(command, "KEMR") == 0) {
    cmd.type = CMD_KEMR;
    cmd.arg_count = 4;
  } else if (strcmp(command, "MEKR") == 0) {
    cmd.type = CMD_MEKR;
    cmd.arg_count = 6;
  } else if (strcmp(command, "SMEKR") == 0) {
    cmd.type = CMD_SMEKR;
    cmd.arg_count = 4;
  } else if (strcmp(command, "KFMRH") == 0) {
    cmd.type = CMD_KFMRH;
    cmd.arg_count = 3;
  } else if (strcmp(command, "MFKRH") == 0) {
    cmd.type = CMD_MFKRH;
    cmd.arg_count = 5;
  } else if (strcmp(command, "SWEEP") == 0) {
    cmd.type = CMD_SWEEP;
    cmd.arg_count = 4;
  } else if (strcmp(command, "RSWEEP") == 0) {
    cmd.type = CMD_RSWEEP;
    cmd.arg_count = 5;
  } else if (strcmp(command, "ARC") == 0) {
    cmd.type = CMD_ARC;
    cmd.arg_count = 10;
  } else if (strcmp(command, "CIRCLE") == 0) {
    cmd.type = CMD_CIRCLE;
    cmd.arg_count = 6;
  } else {
    return CreateErrorCommand("Unknown command");
  }

  for (i = 0; i < cmd.arg_count; i++) {
    while (*ptr && isspace(*ptr)) ptr++;
    if (!*ptr) return CreateErrorCommand("Not enough arguments");
    
    if (sscanf(ptr, "%f", &cmd.args[i]) != 1) {
      return CreateErrorCommand("Invalid argument format");
    }
    
    while (*ptr && !isspace(*ptr)) ptr++;
  }

  return cmd;
}

bdSolid* ExecuteCommands(const char* script, char* error_msg, size_t error_size) {
  bdSolid* solid = NULL;
  bdSolid* new_solid = NULL;
  const char* ptr = script;

  while (*ptr && isspace(*ptr)) ptr++;
  if (!*ptr) {
    return NULL;
  }

  char line[256];
  const char* end;
  int line_num = 1;

  while (*ptr) {
    while (*ptr && isspace(*ptr)) {
      if (*ptr == '\n') line_num++;
      ptr++;
    }
    if (!*ptr) break;

    end = strchr(ptr, '\n');
    if (!end) end = ptr + strlen(ptr);

    size_t len = end - ptr;
    if (len >= sizeof(line)) {
      snprintf(error_msg, error_size, "Line %d: Line too long", line_num);
      return solid;
    }

    strncpy(line, ptr, len);
    line[len] = '\0';

    bdCommand cmd = ParseCommand(line);
    if (cmd.type == CMD_ERROR) {
      snprintf(error_msg, error_size, "Line %d: %s", line_num, cmd.error_msg);
      return solid;
    }

    switch (cmd.type) {
      case CMD_MVFS:
        new_solid = Mvfs((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                         cmd.args[3], cmd.args[4], cmd.args[5]);
        if (solid) Kvfs(solid);
        solid = new_solid;
        break;

      case CMD_KVFS:
        if (!solid) {
          snprintf(error_msg, error_size, "Line %d: No solid exists", line_num);
          return solid;
        }
        Kvfs(solid);
        solid = NULL;
        break;

      case CMD_MEV:
        if (Mev((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                (Id)cmd.args[3], (Id)cmd.args[4], (Id)cmd.args[5],
                (Id)cmd.args[6], cmd.args[7], cmd.args[8], cmd.args[9]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: MEV failed", line_num);
          return solid;
        }
        break;

      case CMD_SMEV:
        if (Smev((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                 (Id)cmd.args[3], cmd.args[4], cmd.args[5], cmd.args[6]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: SMEV failed", line_num);
          return solid;
        }
        break;

      case CMD_KEV:
        if (Kev((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                (Id)cmd.args[3]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: KEV failed", line_num);
          return solid;
        }
        break;

      case CMD_MEF:
        if (Mef((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                (Id)cmd.args[3], (Id)cmd.args[4], (Id)cmd.args[5],
                (Id)cmd.args[6]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: MEF failed", line_num);
          return solid;
        }
        break;

      case CMD_SMEF:
        if (Smef((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                 (Id)cmd.args[3], (Id)cmd.args[4]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: SMEF failed", line_num);
          return solid;
        }
        break;

      case CMD_KEF:
        if (Kef((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                (Id)cmd.args[3]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: KEF failed", line_num);
          return solid;
        }
        break;

      case CMD_RINGMV:
        if (!solid) {
          snprintf(error_msg, error_size, "Line %d: No solid exists", line_num);
          return solid;
        }
        if (Ringmv(solid, (Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                   (Id)cmd.args[3], (int)cmd.args[4]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: RINGMV failed", line_num);
          return solid;
        }
        break;

      case CMD_KEMR:
        if (Kemr((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                 (Id)cmd.args[3]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: KEMR failed", line_num);
          return solid;
        }
        break;

      case CMD_MEKR:
        if (Mekr((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                 (Id)cmd.args[3], (Id)cmd.args[4], (Id)cmd.args[5]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: MEKR failed", line_num);
          return solid;
        }
        break;

      case CMD_SMEKR:
        if (Smekr((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                  (Id)cmd.args[3]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: SMEKR failed", line_num);
          return solid;
        }
        break;

      case CMD_KFMRH:
        if (Kfmrh((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: KFMRH failed", line_num);
          return solid;
        }
        break;

      case CMD_MFKRH:
        if (Mfkrh((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
                  (Id)cmd.args[3], (Id)cmd.args[4]) != SUCCESS) {
          snprintf(error_msg, error_size, "Line %d: MFKRH failed", line_num);
          return solid;
        }
        break;

      case CMD_SWEEP:
        if (!solid) {
          snprintf(error_msg, error_size, "Line %d: No solid exists", line_num);
          return solid;
        }
        Sweep(GetFace(solid, (Id)cmd.args[0]), cmd.args[1], cmd.args[2], cmd.args[3]);
        break;

      case CMD_RSWEEP:
        if (!solid) {
          snprintf(error_msg, error_size, "Line %d: No solid exists", line_num);
          return solid;
        }
        Rsweep(solid, (int)cmd.args[0], cmd.args[1], cmd.args[2], cmd.args[3]);
        break;

      case CMD_ARC:
        Arc((Id)cmd.args[0], (Id)cmd.args[1], (Id)cmd.args[2],
            cmd.args[3], cmd.args[4], cmd.args[5], cmd.args[6],
            cmd.args[7], cmd.args[8], (int)cmd.args[9]);
        break;

      case CMD_CIRCLE:
        new_solid = Circle((Id)cmd.args[0], cmd.args[1], cmd.args[2],
                           cmd.args[3], cmd.args[4], (int)cmd.args[5]);
        if (solid) Kvfs(solid);
        solid = new_solid;
        break;

      default:
        break;
    }

    ptr = end;
    line_num++;
  }

  return solid;
}