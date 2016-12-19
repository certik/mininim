/*
  chopper.h -- chopper module;

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

#ifndef MININIM_CHOPPER_H
#define MININIM_CHOPPER_H

/* dungeon cga */
#define DC_CHOPPER_00 "data/chopper/dc-00.png"
#define DC_CHOPPER_01 "data/chopper/dc-01.png"
#define DC_CHOPPER_02 "data/chopper/dc-02.png"
#define DC_CHOPPER_03 "data/chopper/dc-03.png"
#define DC_CHOPPER_04 "data/chopper/dc-04.png"

/* palace cga */
#define PC_CHOPPER_00 "data/chopper/pc-00.png"
#define PC_CHOPPER_01 "data/chopper/pc-01.png"
#define PC_CHOPPER_02 "data/chopper/pc-02.png"
#define PC_CHOPPER_03 "data/chopper/pc-03.png"
#define PC_CHOPPER_04 "data/chopper/pc-04.png"

/* dungeon ega */
#define DE_CHOPPER_00 "data/chopper/de-00.png"
#define DE_CHOPPER_01 "data/chopper/de-01.png"
#define DE_CHOPPER_02 "data/chopper/de-02.png"
#define DE_CHOPPER_03 "data/chopper/de-03.png"
#define DE_CHOPPER_04 "data/chopper/de-04.png"

/* palace ega */
#define PE_CHOPPER_00 "data/chopper/pe-00.png"
#define PE_CHOPPER_01 "data/chopper/pe-01.png"
#define PE_CHOPPER_02 "data/chopper/pe-02.png"
#define PE_CHOPPER_03 "data/chopper/pe-03.png"
#define PE_CHOPPER_04 "data/chopper/pe-04.png"

/* dungeon vga */
#define DV_CHOPPER_00 "data/chopper/dv-00.png"
#define DV_CHOPPER_01 "data/chopper/dv-01.png"
#define DV_CHOPPER_02 "data/chopper/dv-02.png"
#define DV_CHOPPER_03 "data/chopper/dv-03.png"
#define DV_CHOPPER_04 "data/chopper/dv-04.png"

/* palace vga */
#define PV_CHOPPER_00 "data/chopper/pv-00.png"
#define PV_CHOPPER_01 "data/chopper/pv-01.png"
#define PV_CHOPPER_02 "data/chopper/pv-02.png"
#define PV_CHOPPER_03 "data/chopper/pv-03.png"
#define PV_CHOPPER_04 "data/chopper/pv-04.png"

/* palettable */
#define CHOPPER_BLOOD_00 "data/chopper/blood-00.png"
#define CHOPPER_BLOOD_01 "data/chopper/blood-01.png"
#define CHOPPER_BLOOD_02 "data/chopper/blood-02.png"
#define CHOPPER_BLOOD_03 "data/chopper/blood-03.png"
#define CHOPPER_BLOOD_04 "data/chopper/blood-04.png"

/* variables */
extern struct chopper *chopper;
extern size_t chopper_nmemb;

/* functions */
void load_chopper (void);
void unload_chopper (void);
void register_chopper (struct pos *p);
int compare_choppers (const void *d0, const void *d1);
struct chopper *copy_chopper (struct chopper *to,
                              struct chopper *from);
struct chopper *chopper_at_pos (struct pos *p);
void remove_chopper (struct chopper *c);
bool should_chomp (struct pos *p);
void compute_choppers (void);
void draw_chopper_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
void draw_chopper_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);

#endif	/* MININIM_CHOPPER_H */
