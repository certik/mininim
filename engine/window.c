/*
  window.c -- window module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#include <stdio.h>
#include <error.h>
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/random.h"
#include "physics.h"
#include "kid/kid.h"
#include "pillar.h"
#include "level.h"
#include "room.h"
#include "window.h"

/* dungeon cga */
ALLEGRO_BITMAP *dc_window;

/* dungeon ega */
ALLEGRO_BITMAP *de_window;

/* palace ega */
ALLEGRO_BITMAP *pe_window;

/* dungeon vga */
ALLEGRO_BITMAP *dv_window;

/* palace vga */
ALLEGRO_BITMAP *pv_window;

void
load_window (void)
{
  /* dungeon cga */
  dc_window = load_bitmap (DC_WINDOW);

  /* dungeon ega */
  de_window = load_bitmap (DE_WINDOW);

  /* palace ega */
  pe_window = load_bitmap (PE_WINDOW);

  /* dungeon vga */
  dv_window = load_bitmap (DV_WINDOW);

  /* palace vga */
  pv_window = load_bitmap (PV_WINDOW);
}

void
unload_window (void)
{
  /* dungeon cga */
  al_destroy_bitmap (dc_window);

  /* dungeon ega */
  al_destroy_bitmap (de_window);

  /* palace ega */
  al_destroy_bitmap (pe_window);

  /* dungeon vga */
  al_destroy_bitmap (dv_window);

  /* palace vga */
  al_destroy_bitmap (pv_window);
}

void
draw_window (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *window = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: window = dc_window; break;
    case EGA: window = de_window; break;
    case VGA: window = dv_window; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: window = pe_window; break;
    case VGA: window = pv_window; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (window, bitmap, window_coord (p, &c), 0);
}

struct coord *
window_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 4;
  c->room = p->room;
  return c;
}
