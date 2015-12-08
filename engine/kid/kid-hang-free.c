/*
  kid-hang-free.c -- kid hang free module;

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

static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

void
load_kid_hang_free (void)
{
  /* for symmetry */
}

void
unload_kid_hang_free (void)
{
  /* for symmetry */
}

void
kid_hang_free (void)
{
  kid.oaction = kid.action;
  kid.action = kid_hang_free;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_hang_free)
    kid->i = 5, kid->j = -1, kid->wait = 3, kid->reverse = true;

  /* climb */
  if ((kid->i < 5 || kid->j > -1
         || kid->hang_caller != kid_unclimb)
        && up_key && ! hang_limit) {
    kid_climb ();
    return false;
  }

  /* release */
  if ((! shift_key || hang_limit || get_hanged_con (&kid->f) == NO_FLOOR)
      && (kid->i < 5 || kid->j > -1)) {
    int dir = (kid->f.dir == LEFT) ? -1 : +1;
    hang_limit = false;
    if (con (&hang_pos)->fg != NO_FLOOR
        && kid->i >= 4) {
      place_frame (&kid->f, &kid->f, kid_vjump_frameset[13].frame,
                   &hang_pos, (kid->f.dir == LEFT) ? +7 : PLACE_WIDTH + 9, -8);
      kid_vjump ();
      return false;
    }
    if (crel (&hang_pos, +0, dir)->fg != NO_FLOOR
        && kid->i <= 4) {
      place_frame (&kid->f, &kid->f, kid_vjump_frameset[13].frame,
                   &hang_pos, (kid->f.dir == LEFT) ? +7 : PLACE_WIDTH + 5, -8);
      kid_vjump ();
      return false;
    }
    if (con (&hang_pos)->fg == NO_FLOOR
        && kid->i >= 4) {
      place_frame (&kid->f, &kid->f, kid_fall_frameset[0].frame,
                   &hang_pos, (kid->f.dir == LEFT) ? +16 : PLACE_WIDTH - 16, +12);
      kid_fall ();
      return false;
    }
    if (crel (&hang_pos, +0, dir)->fg == NO_FLOOR
        && kid->i <= 4) {
      place_frame (&kid->f, &kid->f, kid_fall_frameset[0].frame,
                   &hang_pos, (kid->f.dir == LEFT) ? -16 : PLACE_WIDTH + 16, +12);
      kid_fall ();
      return false;
    }
  }

  if (kid->reverse && kid->i > 0) {
    if (kid->i == 4  && kid->j++ > 0) hang_limit = true;
    kid->i--;
  } else if (kid->reverse && kid->i == 0) {
    if (kid->wait == 0) {
      kid->reverse = false; kid->i++;
    } else kid->wait--;
  } else if (! kid->reverse
             && ((kid->j == 0 && kid->i < 12)
                 || (kid->j > 0 && kid->i < 9))) kid->i++;
  else if (! kid->reverse
           && ((kid->j == 0 && kid->i == 12)
               || (kid->j > 0 && kid->i == 9))) {
    kid->reverse = true; kid->i--;
  }

  kid->fo.b = kid_hang_frameset[kid->i].frame;
  kid->fo.dx = (kid->reverse) ? -kid_hang_frameset[kid->i + 1].dx
    : kid_hang_frameset[kid->i].dx;
  kid->fo.dy = (kid->reverse) ? -kid_hang_frameset[kid->i + 1].dy
    : kid_hang_frameset[kid->i].dy;

  if (kid->f.b == kid_hang_14) kid->fo.dx = +0, kid->fo.dy = +1;

  if (kid->reverse && kid->j == 0 && kid->i == 0
      && kid->wait < 3) kid->fo.dy = 0;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  return true;
}

static void
physics_out (struct anim *kid)
{
  struct pos hanged_pos;

  /* depressible floors */
  clear_depressible_floor (kid);
  get_hanged_pos (&kid->f, &hanged_pos);
  press_depressible_floor (&hanged_pos);
}