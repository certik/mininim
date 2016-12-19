/*
  mirror.c -- mirror module;

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

#include "mininim.h"

/* dungeon cga */
ALLEGRO_BITMAP *dc_mirror;

/* palace cga */
ALLEGRO_BITMAP *pc_mirror;

/* dungeon ega */
ALLEGRO_BITMAP *de_mirror;

/* palace ega */
ALLEGRO_BITMAP *pe_mirror;

/* dungeon vga */
ALLEGRO_BITMAP *dv_mirror;

/* palace vga */
ALLEGRO_BITMAP *pv_mirror;

struct mirror *mirror = NULL;
size_t mirror_nmemb = 0;

void
load_mirror (void)
{
  /* dungeon cga */
  dc_mirror = load_bitmap (DC_MIRROR);

  /* palace cga */
  pc_mirror = load_bitmap (PC_MIRROR);

  /* dungeon ega */
  de_mirror = load_bitmap (DE_MIRROR);

  /* palace ega */
  pe_mirror = load_bitmap (PE_MIRROR);

  /* dungeon vga */
  dv_mirror = load_bitmap (DV_MIRROR);

  /* palace vga */
  pv_mirror = load_bitmap (PV_MIRROR);
}

void
unload_mirror (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_mirror);

  /* palace cga */
  destroy_bitmap (pc_mirror);

  /* dungeon ega */
  destroy_bitmap (de_mirror);

  /* palace ega */
  destroy_bitmap (pe_mirror);

  /* dungeon vga */
  destroy_bitmap (dv_mirror);

  /* palace vga */
  destroy_bitmap (pv_mirror);
}

void
draw_mirror (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *mirror = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: mirror = dc_mirror; break;
    case EGA: mirror = de_mirror; break;
    case VGA: mirror = dv_mirror; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: mirror = pc_mirror; break;
    case EGA: mirror = pe_mirror; break;
    case VGA: mirror = pv_mirror; break;
    }
    break;
  }

  /* make mirror black */
  struct rect r;
  new_rect (&r, p->room, PLACE_WIDTH * p->place + 2,
            PLACE_HEIGHT * p->floor + 3,
            PLACE_WIDTH - 10, PLACE_HEIGHT - 16);
  draw_filled_rect (bitmap, &r, BLACK);

  /* draw floor reflex */
  draw_floor_reflex (bitmap, p, em, vm);

  /* draw mirror properly */
  if (vm == VGA) mirror = apply_hue_palette (mirror);
  if (hgc) mirror = apply_palette (mirror, hgc_palette);
  if (peq (p, &mouse_pos))
    mirror = apply_palette (mirror, selection_palette);

  struct coord c;
  draw_bitmapc (mirror, bitmap, mirror_coord (p, &c), 0);
}

void
draw_mirror_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f,
                enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *mirror = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: mirror = dc_mirror; break;
    case EGA: mirror = de_mirror; break;
    case VGA: mirror = dv_mirror; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: mirror = pc_mirror; break;
    case EGA: mirror = pe_mirror; break;
    case VGA: mirror = pv_mirror; break;
    }
    break;
  }

  /* make mirror black */
  struct rect r;
  new_rect (&r, p->room, PLACE_WIDTH * p->place + 2,
            PLACE_HEIGHT * p->floor + 3,
            13, PLACE_HEIGHT - 16);
  draw_filled_rect (bitmap, &r, BLACK);

  /* draw floor reflex */
  draw_floor_reflex (bitmap, p, em, vm);

  ignore_clipping_rectangle_intersection = true;
  /* draw anim */
  if (f) {
    push_clipping_rectangle (bitmap, PLACE_WIDTH * p->place + 2,
                             PLACE_HEIGHT * p->floor + 3,
                             16, PLACE_HEIGHT - 9);
    struct anim *a = get_anim_by_id (f->parent_id);
    struct anim a0 = *a;
    invert_frame_dir (&a0.f, &a0.f);
    a0.f.c.x = (2 * PLACE_WIDTH * p->place + 36)
      - (a->f.c.x + al_get_bitmap_width (a->f.b));
    draw_anim_frame (bitmap, &a0, vm);
    pop_clipping_rectangle ();
  }

  /* draw mirror properly */
  if (vm == VGA) mirror = apply_hue_palette (mirror);
  if (hgc) mirror = apply_palette (mirror, hgc_palette);
  if (peq (p, &mouse_pos))
    mirror = apply_palette (mirror, selection_palette);

  struct coord c;
  int h = al_get_bitmap_height (mirror);
  push_reset_clipping_rectangle (bitmap);
  draw_bitmap_regionc (mirror, bitmap, 0, 0, 22, h, mirror_coord (p, &c), 0);
  pop_clipping_rectangle ();

  ignore_clipping_rectangle_intersection = false;
}

void
draw_floor_reflex (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: floor_right = dc_floor_right; break;
    case EGA: floor_right = de_floor_right; break;
    case VGA: floor_right = dv_floor_right; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: floor_right = pc_floor_right; break;
    case EGA: floor_right = pe_floor_right; break;
    case VGA: floor_right = pv_floor_right; break;
    }
    break;
  }

  if (vm == VGA) floor_right = apply_hue_palette (floor_right);
  if (hgc) floor_right = apply_palette (floor_right, hgc_palette);
  if (peq (p, &mouse_pos))
    floor_right = apply_palette (floor_right, selection_palette);

  struct pos pl; prel (p, &pl, +0, -1);
  struct coord c;
  int h = al_get_bitmap_height (floor_right);
  draw_bitmap_regionc (floor_right, bitmap, 0, 2, 17, h - 9,
                       floor_reflex_coord (&pl, &c), 0);
}

struct coord *
floor_reflex_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1) + 2,
               PLACE_HEIGHT * p->floor + 50);
}

struct coord *
mirror_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place - 3,
               PLACE_HEIGHT * p->floor + 3);
}

struct coord *
mirror_reflex_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 3,
               PLACE_HEIGHT * p->floor + 6);
}
