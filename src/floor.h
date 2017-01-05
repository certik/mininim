/*
  floor.h -- floor module;

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

#ifndef MININIM_FLOOR_H
#define MININIM_FLOOR_H

/* dungeon cga */
#define DC_FLOOR_LEFT "data/floor/dc-left.png"
#define DC_FLOOR_RIGHT "data/floor/dc-right.png"
#define DC_FLOOR_BASE "data/floor/dc-base.png"

extern ALLEGRO_BITMAP *dc_floor_base, *dc_floor_left, *dc_floor_right;

/* palace cga */
#define PC_FLOOR_LEFT "data/floor/pc-left.png"
#define PC_FLOOR_RIGHT "data/floor/pc-right.png"
#define PC_FLOOR_BASE "data/floor/pc-base.png"

extern ALLEGRO_BITMAP *pc_floor_base, *pc_floor_left, *pc_floor_right;

/* dungeon ega */
#define DE_FLOOR_LEFT "data/floor/de-left.png"
#define DE_FLOOR_RIGHT "data/floor/de-right.png"
#define DE_FLOOR_BASE "data/floor/de-base.png"

extern ALLEGRO_BITMAP *de_floor_base, *de_floor_left, *de_floor_right;

/* palace ega */
#define PE_FLOOR_LEFT "data/floor/pe-left.png"
#define PE_FLOOR_RIGHT "data/floor/pe-right.png"
#define PE_FLOOR_BASE "data/floor/pe-base.png"

extern ALLEGRO_BITMAP *pe_floor_base, *pe_floor_left, *pe_floor_right;

/* dungeon vga */
#define DV_FLOOR_LEFT "data/floor/dv-left.png"
#define DV_FLOOR_RIGHT "data/floor/dv-right.png"
#define DV_FLOOR_BASE "data/floor/dv-base.png"

extern ALLEGRO_BITMAP *dv_floor_base, *dv_floor_left, *dv_floor_right;

/* palace vga */
#define PV_FLOOR_LEFT "data/floor/pv-left.png"
#define PV_FLOOR_RIGHT "data/floor/pv-right.png"
#define PV_FLOOR_BASE "data/floor/pv-base.png"

extern ALLEGRO_BITMAP *pv_floor_base, *pv_floor_left, *pv_floor_right;

void load_floor (void);
void unload_floor (void);
void draw_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm);
void draw_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm);
struct coord *floor_base_coord (struct pos *p, struct coord *c);
struct coord *floor_left_coord (struct pos *p, struct coord *c);
struct coord *floor_right_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_FLOOR_H */
