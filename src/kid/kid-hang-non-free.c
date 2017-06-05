/*
  kid-hang-non-free.c -- kid hang non-free module;

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

static bool flow (struct actor *k);
static bool physics_in (struct actor *k);
static void physics_out (struct actor *k);

void
kid_hang_non_free (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_hang_non_free;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  /* change hang style in case tiles change on the fly (by level
     editing for example) */
  if (is_free (&k->hang_pos, k->f.dir)) {
    kid_hang (k);
    return false;
  }

  if (k->oaction != kid_hang_non_free)
    k->i = 3, k->wait = 0, k->reverse = false;

  bool hang_back = ((k->f.dir == LEFT) ? k->key.right : k->key.left)
    && ! k->key.up && k->key.shift;

  int back_dir = (k->f.dir == LEFT) ? RIGHT : LEFT;

  /* hang back */
  if (movements == NATIVE_MOVEMENTS
      && k->i == 4 && k->reverse
      && hang_back && is_hangable (&k->hang_pos, back_dir)) {
    play_audio (&hang_on_fall_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_HANG);
    kid_turn (k);
    return false;
  }

  if ((! k->key.shift && (k->reverse || k->i > 3))
      || k->hang_limit
      || get_hanged_tile (&k->hang_pos, k->f.dir) == NO_FLOOR
      || ! is_hangable (&k->hang_pos, k->f.dir)) {
    if (is_traversable (&k->hang_pos)) {
      struct rect r; actor_rect (&r, k, "KID", "HANG", "NON_FREE_FALL");
      place_actor (k, &k->hang_pos, r.c.x, r.c.y, "KID", "FALL", 0);
      kid_fall (k);
      k->hang_limit = false;
      return false;
    }
    struct rect r; actor_rect (&r, k, "KID", "HANG", "NON_FREE_VJUMP");
    place_actor (k, &k->hang_pos, r.c.x, r.c.y, "KID", "VJUMP", 13);
    kid_vjump (k);
    k->hang_limit = false;
    return false;
  } if (k->key.up
        && ! ((k->key.left || k->key.right)
              && k->hang_caller == kid_unclimb)
        && ! k->hang_limit) {
    kid_climb (k);
    return false;
  }

  if (! k->reverse && k->i < 6) k->i++;
  else if (! k->reverse && k->i == 6) k->reverse = true;
  else if (k->reverse && k->i > 4 && k->wait == 0)
    k->i--, k->wait = 1;
  else if (k->reverse && k->i >= 4 && k->wait > 0) k->wait--;

  k->fo.b = actor_bitmap (k, "KID", "HANG", k->i);
  k->fo.dx = (k->reverse) ? -actor_dx (k, "KID", "HANG", k->i + 1)
    : actor_dx (k, "KID", "HANG", k->i);
  k->fo.dy = (k->reverse) ? -actor_dy (k, "KID", "HANG", k->i + 1)
    : actor_dy (k, "KID", "HANG", k->i);

  if (k->reverse && k->wait == 0) {
    k->fo.dx = 0;
    k->fo.dy = 0;
  }

  return true;
}

static bool
physics_in (struct actor *k)
{
  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  keep_depressible_floor (k);

  /* sound */
  if (! k->reverse && k->i == 4 && k->key.shift && ! k->key.up
      && ! k->hang_limit)
    play_audio (&hit_wall_audio, NULL, k->id);
}
