/*
  repl.c -- Lua REPL module;

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

/******************************************************************************
* Copyright (C) 1994-2006 Tecgraf, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#include "mininim.h"

#define PROMPT "MININIM> "
#define PROMPT2 ".......>> "
#define LUA_HISTORY_ENV "MININIM_HISTORY"
#define LUA_HISTSIZE_ENV "MININIM_HISTSIZE"

static char *myhist;
static int myhistsize;

static char *lhandler_line;

static void
lhandler (char *line)
{
  rl_callback_handler_remove ();
  if (! line) {
    fprintf (rl_outstream, "\n");
    lhandler_line = xasprintf ("");
  }
  else lhandler_line = line;
}

/* Read a line from the terminal with line editing */
static int lua_readline(lua_State *repl_L, const char *prompt)
{
  rl_callback_handler_install (prompt, lhandler);

  struct timeval timeout;
  fd_set set;
  al_unlock_mutex (repl_mutex);
  while (! lhandler_line && ! al_get_thread_should_stop (repl_thread)) {
    FD_ZERO (&set);
    FD_SET (fileno (rl_instream), &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    if (select (FD_SETSIZE, &set, NULL, NULL, &timeout) > 0)
      rl_callback_read_char ();
  }

  al_lock_mutex (repl_mutex);

  if (! lhandler_line) return 0;

  lua_pushstring(repl_L, lhandler_line);
  lua_pushliteral(repl_L, "\n");
  lua_concat(repl_L, 2);
  free(lhandler_line);
  lhandler_line = NULL;
  return 1;
}

/* Add a line to the history */
static void lua_saveline(lua_State *repl_L, const char *s)
{
  const char *p;
  for (p = s; isspace(*p); p++) ;
  if (*p) {
    size_t n = strlen(s)-1;
    if (s[n] != '\n') {
      add_history(s);
    } else {
      lua_pushlstring(repl_L, s, n);
      s = lua_tostring(repl_L, -1);
      add_history(s);
      lua_pop(repl_L, 1);
    }
  }
}

/* Reserved lua keywords */
static const char * const reskeywords[] = {
  "and", "break", "do", "else", "elseif", "end", "false",
  "for", "function", "if", "in", "local", "nil", "not", "or",
  "repeat", "return", "then", "true", "until", "while", NULL
};

static int valididentifier(const char *s)
{
  if (!(isalpha(*s) || *s == '_')) return 0;
  for (s++; *s; s++) if (!(isalpha(*s) || isdigit(*s) || *s == '_')) return 0;
  return 1;
}

/* Dynamically resizable match list */
typedef struct {
  char **list;
  size_t idx, allocated, matchlen;
} dmlist;

/* Add prefix + string + suffix to list and compute common prefix */
static int dmadd(dmlist *ml, const char *p, size_t pn, const char *s, int suf)
{
  char *t = NULL;

  if (ml->idx+1 >= ml->allocated &&
      !(ml->list = realloc(ml->list, sizeof(char *)*(ml->allocated += 32))))
    return -1;

  if (s) {
    size_t n = strlen(s);
    if (!(t = (char *)malloc(sizeof(char)*(pn+n+(suf?2:1))))) return 1;
    memcpy(t, p, pn);
    memcpy(t+pn, s, n);
    n += pn;
    t[n] = suf;
    if (suf) t[++n] = '\0';

    if (ml->idx == 0) {
      ml->matchlen = n;
    } else {
      size_t i;
      for (i = 0; i < ml->matchlen && i < n && ml->list[1][i] == t[i]; i++) ;
      ml->matchlen = i;	/* Set matchlen to common prefix */
    }
  }

  ml->list[++ml->idx] = t;
  return 0;
}

/* Get __index field of metatable of object on top of stack */
static int getmetaindex(lua_State *repl_L)
{
  if (!lua_getmetatable(repl_L, -1)) { lua_pop(repl_L, 1); return 0; }
  lua_pushstring(repl_L, "__index");
  lua_rawget(repl_L, -2);
  lua_replace(repl_L, -2);
  if (lua_isnil(repl_L, -1) || lua_rawequal(repl_L, -1, -2)) { lua_pop(repl_L, 2); return 0; }
  lua_replace(repl_L, -2);
  return 1;
} /* 1: obj -- val, 0: obj -- */

/* Get field from object on top of stack. Avoid calling metamethods */
static int safegetfield(lua_State *repl_L, const char *s, size_t n)
{
  int i = 20; /* Avoid infinite metatable loops */
  do {
    if (lua_istable(repl_L, -1)) {
      lua_pushlstring(repl_L, s, n);
      lua_rawget(repl_L, -2);
      if (!lua_isnil(repl_L, -1)) { lua_replace(repl_L, -2); return 1; }
      lua_pop(repl_L, 1);
    }
  } while (--i > 0 && getmetaindex(repl_L));
  lua_pop(repl_L, 1);
  return 0;
} /* 1: obj -- val, 0: obj -- */

/* Completion function */
static char **mycomplete(const char *text, int start, int end)
{
  dmlist ml;
  const char *s;
  size_t i, n, dot;
  int savetop;

  if (!(text[0] == '\0' || isalpha(text[0]) || text[0] == '_')) return NULL;

  ml.list = NULL;
  ml.idx = ml.allocated = ml.matchlen = 0;

  savetop = lua_gettop(repl_L);
  lua_pushvalue(repl_L, LUA_GLOBALSINDEX);
  for (n = (size_t)(end-start), i = dot = 0; i < n; i++)
    if (text[i] == '.' || text[i] == ':') {
      if (!safegetfield(repl_L, text+dot, i-dot)) goto error; /* invalid prefix */
      dot = i+1; /* points to first char after dot/colon */
    }

  /* Add all matches against keywords if there is no dot/colon */
  if (dot == 0)
    for (i = 0; (s = reskeywords[i]) != NULL; i++)
      if (!strncmp(s, text, n) && dmadd(&ml, NULL, 0, s, ' ')) goto error;

  /* Add all valid matches from all tables/metatables */
  i = 20; /* Avoid infinite metatable loops */
  do {
    if (lua_istable(repl_L, -1))
      for (lua_pushnil(repl_L); lua_next(repl_L, -2); lua_pop(repl_L, 1))
	if (lua_type(repl_L, -2) == LUA_TSTRING) {
	  s = lua_tostring(repl_L, -2);
	  /* Only match names starting with '_' if explicitly requested */
	  if (!strncmp(s, text+dot, n-dot) && valididentifier(s) &&
	      (*s != '_' || text[dot] == '_')) {
	    int suf = ' '; /* default suffix is a space */
	    switch (lua_type(repl_L, -1)) {
	    case LUA_TTABLE:	suf = '.'; break; /* No way to guess ':' */
	    case LUA_TFUNCTION:	suf = '('; break;
	    case LUA_TUSERDATA:
	      if (lua_getmetatable(repl_L, -1)) { lua_pop(repl_L, 1); suf = ':'; }
	      break;
	    }
	    if (dmadd(&ml, text, dot, s, suf)) goto error;
	  }
	}
  } while (--i > 0 && getmetaindex(repl_L));

  if (ml.idx > 1) {
    /* list[0] holds the common prefix of all matches (may be "") */
    if (!(ml.list[0] = (char *)malloc(sizeof(char)*(ml.matchlen+1)))) {
error:
      lua_settop(repl_L, savetop);
      return NULL;
    }
    memcpy(ml.list[0], ml.list[1], ml.matchlen);
    ml.list[0][ml.matchlen] = '\0';
    /* Add the NULL list terminator */
    if (dmadd(&ml, NULL, 0, NULL, 0)) goto error;
  } else if (ml.idx == 1) {
    ml.list[0] = ml.list[1];		/* Only return common prefix */
    ml.list[1] = NULL;
  }

  lua_settop(repl_L, savetop);
  return ml.list;
}

/* Initialize library */
static void lua_initline(lua_State *repl_L, char *pname)
{
  char *s;

  /* This allows for $if lua ... $endif in ~/.inputrc */
  rl_readline_name = pname;
  /* Break words at every non-identifier character except '.' and ':' */
  rl_completer_word_break_characters =
    "\t\r\n !\"#$%&'()*+,-/;<=>?@[\\]^`{|}~";
  rl_completer_quote_characters = "\"'";
  rl_completion_append_character = '\0';
  rl_attempted_completion_function = mycomplete;
  rl_initialize();

  /* Start using history, optionally set history size and load history file */
  using_history();
  if ((s = getenv(LUA_HISTSIZE_ENV)) &&
      (myhistsize = atoi(s))) stifle_history(myhistsize);
  if ((myhist = getenv(LUA_HISTORY_ENV))) read_history(myhist);
}

/* Finalize library */
static void lua_exitline(lua_State *repl_L)
{
  /* Optionally save history file */
  if (myhist) write_history(myhist);
}

/* These bits are added for Debian's -P functionality */

static int done_path = 0;
static int suppress_path = 0;

static const char* paths[] = {
  "~/.lua50",
  "~/share/lua/50", // backward compatibility for the lua package
  "~/share/lua50",
  "/usr/share/lua/50",
  "/usr/share/lua50", // backward compatibility for the lua package
  "/usr/local/share/lua50",
  NULL
};

static const char* cpaths[] = {
  "~/.lua50",
  "~/lib/lua/50",
  "~/lib/lua50", // backward compatibility for the lua package
  "/usr/lib/lua/50",
  "/usr/lib/lua50", // backward compatibility for the lua package
  "/usr/local/lib/lua50",
  NULL
};

static void do_path(lua_State *repl_L)
{
  const char** p = paths;
  int any;
  if( done_path || suppress_path ) return;
  if( ! repl_L ) return;
  done_path = 1;
  lua_pushliteral(repl_L,"LUA_PATH");
  lua_pushliteral(repl_L,"");
  while( *p ) {
    any = 0;
    if( **p == '~' ) {
      const char* home = getenv("HOME");
      if( home ) {
        lua_pushstring(repl_L,home);
        lua_pushstring(repl_L,*p+1);
        lua_pushliteral(repl_L,"/?.lua;");
        lua_pushstring(repl_L,home);
        lua_pushstring(repl_L,*p+1);
        lua_pushliteral(repl_L,"/?;");
        any = 6;
      }
    } else {
      lua_pushstring(repl_L,*p);
      lua_pushliteral(repl_L,"/?.lua;");
      lua_pushstring(repl_L,*p);
      lua_pushliteral(repl_L,"/?;");
      any = 4;
    }
    if( any ) {
      lua_concat(repl_L,any+1);
    }
    p++;
  }
  lua_pushliteral(repl_L, "?.lua;?");
  lua_concat(repl_L,2);
  lua_settable(repl_L, LUA_GLOBALSINDEX);
  p=cpaths;
  lua_pushliteral(repl_L,"LUA_CPATH");
  lua_pushliteral(repl_L,"");
  while( *p ) {
    any = 0;
    if( **p == '~' ) {
      const char* home = getenv("HOME");
      if( home ) {
        lua_pushstring(repl_L,home);
        lua_pushstring(repl_L,*p+1);
        lua_pushliteral(repl_L,"/?.so;");
        lua_pushstring(repl_L,home);
        lua_pushstring(repl_L,*p+1);
        lua_pushliteral(repl_L,"/?;");
        any = 6;
      }
    } else {
      lua_pushstring(repl_L,*p);
      lua_pushliteral(repl_L,"/?.so;");
      lua_pushstring(repl_L,*p);
      lua_pushliteral(repl_L,"/?;");
      any = 4;
    }
    if( any ) {
      lua_concat(repl_L,any+1);
    }
    p++;
  }
  lua_pushliteral(repl_L, "?.so;?");
  lua_concat(repl_L,2);
  lua_settable(repl_L, LUA_GLOBALSINDEX);
}


static void lstop (lua_State *repl_L, lua_Debug *ar) {
  (void)ar;  /* unused arg. */
  lua_sethook(repl_L, NULL, 0, 0);
  lua_pushnil(repl_L);
  lua_error(repl_L);
}


static void laction (int i) {
  lua_sethook(repl_L, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}

static void l_message (const char *pname, const char *msg) {
  if (pname) fprintf(rl_outstream, "%s: ", pname);
  fprintf(rl_outstream, "%s\n", msg);
}


static int report (lua_State *repl_L, int status) {
  const char *msg;
  if (status) {
    msg = lua_tostring(repl_L, -1);
    if (msg == NULL) {
      const char *str;
      lua_getglobal(repl_L, "LUA_DEFAULT_ERROR");  /* try global variable */
      str = lua_tostring(repl_L, -1);
      lua_pop(repl_L, 1);
      if (str) {
        if (*str != '\0') msg = str;
      } else msg = "(error with no message)";
    }
    if (msg) l_message(program_name, msg);
    lua_pop(repl_L, 1);
  }
  return status;
}

static void sig_catch(int sig, void (*handler)(int))
{
  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(sig, &sa, 0);         /* XXX ignores errors */
}


static int lcall (lua_State *repl_L, int narg, int clear) {
  int status;
  int base = lua_gettop(repl_L) - narg;  /* function index */
  do_path(repl_L);
  //lua_settop(repl_L,base);
  lua_pushliteral(repl_L, "_TRACEBACK");
  lua_rawget(repl_L, LUA_GLOBALSINDEX);  /* get traceback function */
  lua_insert(repl_L, base);  /* put it under chunk and args */
  sig_catch(SIGINT, laction);
  status = lua_pcall(repl_L, narg, (clear ? 0 : LUA_MULTRET), base);
  sig_catch(SIGINT, SIG_DFL);
  lua_remove(repl_L, base);  /* remove traceback function */
  return status;
}


static const char *get_prompt (lua_State *repl_L, int firstline) {
  const char *p = NULL;
  lua_pushstring(repl_L, firstline ? "_PROMPT" : "_PROMPT2");
  lua_rawget(repl_L, LUA_GLOBALSINDEX);
  p = lua_tostring(repl_L, -1);
  if (p == NULL) p = (firstline ? PROMPT : PROMPT2);
  lua_pop(repl_L, 1);  /* remove global */
  return p;
}


static int incomplete (lua_State *repl_L, int status) {
  if (status == LUA_ERRSYNTAX &&
         strstr(lua_tostring(repl_L, -1), "near `<eof>'") != NULL) {
    lua_pop(repl_L, 1);
    return 1;
  }
  else
    return 0;
}


static int load_string (lua_State *repl_L) {
  int status;
  lua_settop(repl_L, 0);
  if (lua_readline(repl_L, get_prompt(repl_L, 1)) == 0)  /* no input? */
    return -1;
  if (lua_tostring(repl_L, -1)[0] == '=') {  /* line starts with `=' ? */
    lua_pushfstring(repl_L, "return %s", lua_tostring(repl_L, -1)+1);/* `=' -> `return' */
    lua_remove(repl_L, -2);  /* remove original line */
  }
  for (;;) {  /* repeat until gets a complete line */
    status = luaL_loadbuffer(repl_L, lua_tostring(repl_L, 1), lua_strlen(repl_L, 1), "=stdin");
    if (!incomplete(repl_L, status)) break;  /* cannot try to add lines? */
    if (lua_readline(repl_L, get_prompt(repl_L, 0)) == 0)  /* no more input? */
      return -1;
    lua_concat(repl_L, lua_gettop(repl_L));  /* join lines */
  }
  lua_saveline(repl_L, lua_tostring(repl_L, 1));
  lua_remove(repl_L, 1);  /* remove line */
  return status;
}


static void manual_input (lua_State *repl_L) {
  int status;
  lua_initline(repl_L, "mininim");
  do_path(repl_L);
  while (! al_get_thread_should_stop (repl_thread)) {
    status = load_string(repl_L);
    if (status == -1) {
      if (! al_get_thread_should_stop (repl_thread))
        fprintf (rl_outstream, "\n");
      continue;
    }
    if (status == 0) status = lcall(repl_L, 0, 0);
    report(repl_L, status);
    if (status == 0 && lua_gettop(repl_L) > 0) {  /* any result to print? */
      lua_getglobal(repl_L, "print");
      lua_insert(repl_L, 1);
      if (lua_pcall(repl_L, lua_gettop(repl_L)-1, 0, 0) != 0)
        l_message(program_name,
                  lua_pushfstring(repl_L, "error calling `print' (%s)",
                                  lua_tostring(repl_L, -1)));
    }
  }
  lua_settop(repl_L, 0);  /* clear stack */
  fputs("\n", rl_outstream);
  lua_exitline(repl_L);
}

static int pmain (lua_State *repl_L) {
  lua_pop (repl_L, 1);
  do_path(repl_L);
  manual_input(repl_L);
  return 0;
}

ALLEGRO_MUTEX *repl_mutex;
ALLEGRO_THREAD *repl_thread;
lua_State *repl_L;
int repl_thread_ref = LUA_NOREF;

void *
repl (ALLEGRO_THREAD *thread, void *repl_L)
{
  al_lock_mutex (repl_mutex);

  rl_instream = stdin;
  rl_outstream = stderr;

  fprintf (rl_outstream,
           "MININIM %s %s\n\n", VERSION, about_dialog.text);
  fprintf (rl_outstream, LUA_VERSION " " LUA_COPYRIGHT "\n");

  int status;
  status = lua_cpcall (repl_L, &pmain, NULL);
  report (repl_L, status);

  rl_callback_handler_remove ();

  al_unlock_mutex (repl_mutex);
  return NULL;
}
