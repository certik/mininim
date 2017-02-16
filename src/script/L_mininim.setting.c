/*
  L_mininim.setting.c -- mininim.setting script module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mininim.h"

static int __eq (lua_State *L);
static int __index (lua_State *L);
static int __newindex (lua_State *L);
static int __tostring (lua_State *L);

void
define_L_mininim_setting (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_SETTING);

  lua_pushstring (L, "__eq");
  lua_pushcfunction (L, __eq);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_pushstring (L, "__tostring");
  lua_pushcfunction (L, __tostring);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

int
__eq (lua_State *L)
{
  lua_pushboolean (L, true);
  return 1;
}

int
__index (lua_State *L)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "audio_mode")) {
      lua_pushstring (L, audio_mode);
      return 1;
    } else if (! strcasecmp (key, "video_mode")) {
      lua_pushstring (L, video_mode);
      return 1;
    } else if (! strcasecmp (key, "env_mode")) {
      lua_pushstring (L, env_mode);
      return 1;
    } else break;
  default: break;
  }

  lua_pushnil (L);
  return 1;
}

int
__newindex (lua_State *L)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "audio_mode")) {
      L_set_string_var (L, 3, &audio_mode);
      return 0;
    } else if (! strcasecmp (key, "video_mode")) {
      L_set_string_var (L, 3, &video_mode);
      return 0;
    } else if (! strcasecmp (key, "env_mode")) {
      L_set_string_var (L, 3, &env_mode);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}

int
__tostring (lua_State *L)
{
  lua_pushstring (L, "MININIM SETTINGS INTERFACE");
  return 1;
}

void
set_string_var (char **var, const char *value)
{
  if (*var) al_free (*var);
  *var = xasprintf ("%s", value);
}

void
L_set_string_var (lua_State *L, int index, char **var)
{
  const char *value = lua_tostring (L, index);
  if (value) set_string_var (var, value);
}