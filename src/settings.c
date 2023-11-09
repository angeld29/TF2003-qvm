/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 */
#include "g_local.h"

/*typedef enum
{
    SET_HWGUY_SB,
    SET_SBAR_RES,
    SET_SBAR_SIZE,
    SET_DISCARD_SHELLS,
    SET_DISCARD_NAILS,
    SET_DISCARD_ROCKETS,
    SET_DISCARD_CELLS,
    SET_BITS,
    SET_CLASSHELP,
    SET_MULTISCAN,
    SET_EXEC_CLASS,
    SET_EXEC_MAP,
    SET_TAKE_SSHOT,
    SET_GREN_SOUND,
    _SET_LAST,
} cl_setting_id_t;*/

typedef enum {
  CS_T_BOOL,
  CS_T_INT,
} cl_settings_type_t;

typedef struct cl_settings_s {
  const char *key;
  const char *key2;
  const char *name;
  const char *desc;
  // cl_setting_id_t id;
  cl_settings_type_t type;
  int bit;
  int defval;
} cl_settings_t;

static const cl_settings_t cl_set[] = {
    {"sb",         NULL, "HWGuy shells backup",      "",     CS_T_INT, FOFS( assault_min_shells ), DEFAULT_ASSAULT_MIN_SHELLS, },
    {"sbar_res",   "sbr", "StatusBar Resolution",  "", CS_T_INT, FOFS( StatusBarRes ), 0, },
    {"sbar_size",  "sbs", "StatusBar Size",        "", CS_T_INT, FOFS( StatusBarSize ), 0, },
    {"mxs",        "1", "Discard shells",      "",     CS_T_INT, FOFS(discard_shells), -1, },
    {"mxn",        "2", "Discard nails",       "",      CS_T_INT, FOFS(discard_nails), -1, },
    {"mxr",        "3", "Discard rockets",     "",    CS_T_INT, FOFS(discard_rockets), -1, },
    {"mxc",        "4", "Discard cells",       "",      CS_T_INT, FOFS(discard_cells), -1, },
    {"set_bits",   "s", "Settings  Bits",        "",         CS_T_INT, FOFS(settings_bits), 0, },
    {"classhelp",  "ch", "Show Classhelp",        "",        CS_T_BOOL, TF_CLASS_HELP_MASK, 1, },
    {"multiscan",  "ms", "Multiscan",        "",             CS_T_BOOL, TF_MULTISCAN_MASK, 1, },
    {"exec_class", "ec", "Exec Class config",        "",     CS_T_BOOL, TF_EXEC_CLASS_MASK, 0, },
    {"exec_map",   "em", "Exec Map config",        "",       CS_T_BOOL, TF_EXEC_MAP_MASK, 0, },
    {"take_sshot", NULL, "Screenshot ScoreTable",        "", CS_T_BOOL, TF_TAKE_SSHOT_MASK, 0, },
    {"grensound",  NULL, "Play sound on grenade prime",        "",  CS_T_BOOL,  TF_INTERNAL_GRENSOUND, 0, },
    {"usemapflaginfo",  NULL, "Use map defined flag info",        "",  CS_T_BOOL,  TF_INTERNAL_USEMAPFLAGINFO, 1, },
};

#define CL_SET_NUM sizeof(cl_set) / sizeof(cl_set[0])

typedef struct cl_set_map_s {
  const char *key;
  int idx;
} cl_set_map_t;

int comp(const void *i, const void *j) { return strcmp(((cl_set_map_t *)(i))->key, ((cl_set_map_t *)(j))->key); }

static const cl_settings_t *get_set_info(const char *key) {
  static cl_set_map_t key_map[CL_SET_NUM * 2];
  cl_set_map_t *f = NULL;
  static int num_set = 0;
  cl_set_map_t key_s;
  key_s.key = key;
  if (!num_set) {
    int i;
    for (i = 0; i < CL_SET_NUM; i++) {
      key_map[num_set].key = cl_set[i].key;
      key_map[num_set++].idx = i;
      if (cl_set[i].key2) {
        key_map[num_set].key = cl_set[i].key2;
        key_map[num_set++].idx = i;
      }
    }
    qsort(key_map, num_set, sizeof(key_map[0]), comp);
  }
  if (!key)
    return NULL;
  f = bsearch(&key_s, key_map, num_set, sizeof(key_map[0]), comp);

  if (!f)
    return NULL;
  return &cl_set[f->idx];
}

void PrintClientSettingNew(gedict_t *p, const char *key) {

  const cl_settings_t *s;
  int val;

  if (!key)
    return;
  s = get_set_info(key);
  if (!s) {
    G_sprint(p, 2, "Key '%s' not found\n", key);
    return;
  }
  G_sprint(p, 2, "%s: ", s->name);
  if (s->type == CS_T_INT) {
    int val = *(int *)((byte *)p + s->bit);
    G_sprint(p, PRINT_CHAT, "%d\n", val);
  } else {
    G_sprint(p, PRINT_CHAT, p->settings_bits & s->bit ? "On\n" : "Off\n");
  }
}
qboolean SetClientSetting(gedict_t *p, const char *key, const char *value) {
  const cl_settings_t *s;
  int val;

  s = get_set_info(key);
  if (!s)
    return 0;

  if (!value || !value[0])
    val = s->defval;
  else
    val = atoi(value);

  if (s->type == CS_T_INT) {
    *(int *)((byte *)p + s->bit) = val;
  } else {
    if (streq(value, "on")) {
      p->settings_bits |= s->bit;
    } else {
      if (streq(value, "off")) {
        p->settings_bits -= p->settings_bits & s->bit;
      } else {
        if (val)
          p->settings_bits |= s->bit;
        else
          p->settings_bits -= p->settings_bits & s->bit;
      }
    }
  }
  p->StatusRefreshTime = g_globalvars.time + 0.2;
  return 1;
}

void Client_Set_Cmd() {
  char key[20];
  char value[20];
  int argc;

  argc = trap_CmdArgc();
  if (argc < 2) {
    int i;
    G_sprint(self, 2, "Player settings\n");
    for (i = 0; i < CL_SET_NUM; i++) {
      G_sprint(self, 2, "%s: ", cl_set[i].key);
      PrintClientSettingNew(self, cl_set[i].key);
    }
    return;
  }
  trap_CmdArgv(1, key, sizeof(key));
  Q_strlwr(key);
  if (argc < 3) {
    PrintClientSettingNew(self, key);
    return;
  }
  trap_CmdArgv(2, value, sizeof(value));
  if (SetClientSetting(self, key, value))
    PrintClientSettingNew(self, key);
}
//=======================
void ParseUserInfo() {
  char value[100];
  int i;
  self->assault_min_shells = DEFAULT_ASSAULT_MIN_SHELLS;
  self->StatusBarRes = 0;
  self->StatusBarSize = 0;
  self->discard_shells = -1;
  self->discard_nails = -1;
  self->discard_rockets = -1;
  self->discard_cells = -1;
  self->settings_bits = TF_CLASS_HELP_MASK | TF_AUTOID_MASK | TF_INTERNAL_GRENSOUND | TF_INTERNAL_USEMAPFLAGINFO;
  self->internal_settings_bits = 0;
  self->take_sshot = 0;
  for (i = 0; i < CL_SET_NUM; i++) {
    if (GetInfokeyString(self, cl_set[i].key, cl_set[i].key2, value, sizeof(value), NULL))
      SetClientSetting(self, cl_set[i].key, value);
  }
}
