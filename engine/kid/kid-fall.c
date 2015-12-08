/*
  kid-fall.c -- kid fall module;

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
#include "prince.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "kid.h"

struct frameset kid_fall_frameset[KID_FALL_FRAMESET_NMEMB];

static void init_kid_fall_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_fall_13, *kid_fall_14, *kid_fall_15,
  *kid_fall_16, *kid_fall_17;

static void
init_kid_fall_frameset (void)
{
  struct frameset frameset[KID_FALL_FRAMESET_NMEMB] =
    {{kid_fall_13,+0,+0},{kid_fall_14,+0,+5},{kid_fall_15,+0,+10},
     {kid_fall_16,+0,+11},{kid_fall_17,+0,+20}};

  memcpy (&kid_fall_frameset, &frameset,
          KID_FALL_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_fall (void)
{
  /* bitmaps */
  kid_fall_13 = load_bitmap (KID_FALL_13);
  kid_fall_14 = load_bitmap (KID_FALL_14);
  kid_fall_15 = load_bitmap (KID_FALL_15);
  kid_fall_16 = load_bitmap (KID_FALL_16);
  kid_fall_17 = load_bitmap (KID_FALL_17);

  /* frameset */
  init_kid_fall_frameset ();
}

void
unload_kid_fall (void)
{
  al_destroy_bitmap (kid_fall_13);
  al_destroy_bitmap (kid_fall_14);
  al_destroy_bitmap (kid_fall_15);
  al_destroy_bitmap (kid_fall_16);
  al_destroy_bitmap (kid_fall_17);
}

void
kid_fall (void)
{
  kid.oaction = kid.action;
  kid.action = kid_fall;
  kid.f.flip = (kid.f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_fall) kid->i = -1;

  kid->i++;

  kid->fo.b = kid_fall_frameset[kid->i > 4 ? 4 : kid->i].frame;
  kid->fo.dx = kid_fall_frameset[kid->i > 4 ? 4 : kid->i].dx;
  kid->fo.dy = kid_fall_frameset[kid->i > 4 ? 4 : kid->i].dy;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc;
  struct pos np, pbf, pbb, pmbo, fall_pos, npmbo, npmbo_nf;
  enum confg cmbo;
  struct frame nf;

  if (kid->i == 0) {
    survey (_bf, pos, &kid->f, &nc, &pbf, &np);
    survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
    survey (_bb, pos, &kid->f, &nc, &pbb, &np);

    fall_pos.room = -1;

    if (is_strictly_traversable (&pbf)) fall_pos = pbf;
    else if (is_strictly_traversable (&pmbo)) fall_pos = pmbo;
    else if (is_strictly_traversable (&pbb)) fall_pos = pbb;

    if (fall_pos.room != - 1)
      place_frame (&kid->f, &kid->f, kid_fall_frameset[0].frame,
                   &fall_pos,
                   (kid->f.dir == LEFT) ? PLACE_WIDTH - 12 : +16,
                   (kid->f.dir == LEFT) ? 23 : 27);
  }

  /* help kid hang */
  survey (_bf, pos, &kid->f, &nc, &pbf, &np);
  if (! is_strictly_traversable (&pbf)) inertia = 0;

  /* fall speed */
  if (kid->i > 0) kid->fo.dx = -inertia;
  if (kid->i > 4) {
    int speed = +21 + 3 * (kid->i - 5);
    kid->fo.dy = (speed > 33) ? 33 : speed;
  }

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, false))
    kid->fo.dx = 0;

  /* hang */
  if (kid->i > 2 && can_hang (&kid->f) && shift_key && ! hang_limit) {
    sample_hang_on_fall = true;
    kid_hang ();
    return false;
  }

  /* land on ground */
  cmbo = survey (_mbo, pos, &kid->f, &nc, &np, &npmbo)->fg;
  next_frame (&kid->f, &nf, kid->f.b, 0, 34);
  survey (_mbo, pos, &nf, &nc, &np, &npmbo_nf);

  if (kid->i > 2
      && cmbo != NO_FLOOR
      && npmbo.floor != npmbo_nf.floor) {
    inertia = 0;

    pos2view (&pbf, &pbf);
    kid->f.c.room = pbf.room;
    kid->f.c.x += (kid->f.dir == LEFT) ? -6 : +6;
    kid->f.c.y = PLACE_HEIGHT * pbf.floor + 15;
    kid->f.b = kid_normal_00;

    if (kid->i >= 8) {
      kid_current_lives--;
      uncouch_slowly = true;
    } else uncouch_slowly = false;
    if (kid->i > 3) sample_hit_ground = true;
    shake_loose_floor_row (&pbf);
    kid_couch ();
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  clear_depressible_floor (kid);
}

bool
is_kid_fall (struct frame *f)
{
  return f->b == kid_fall_13
    || f->b == kid_fall_14
    || f->b == kid_fall_15
    || f->b == kid_fall_16
    || f->b == kid_fall_17;
}