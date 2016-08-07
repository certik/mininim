/*
  physics.c -- physics module;

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

struct con *
con (struct pos *p)
{
  struct pos np; npos (p, &np);
  return &np.l->con[np.room][np.floor][np.place];
}

struct con *
crel (struct pos *p, int floor, int place)
{
  struct pos pr;
  return con (prel (p, &pr, floor, place));
}

bool
strictly_traversable_cs (enum confg t)
{
  return t == NO_FLOOR
    || t == BIG_PILLAR_TOP
    || t == TCARPET
    || arch_top_cs (t);
}

bool
is_strictly_traversable (struct pos *p)
{
  enum confg t = con (p)->fg;
  return strictly_traversable_cs (t);
}

bool
traversable_cs (enum confg t)
{
  return strictly_traversable_cs (t)
    || t == LOOSE_FLOOR;
}

bool
is_traversable (struct pos *p)
{
  return traversable_cs (con (p)->fg);
}

bool
is_pillar (struct pos *p)
{
  enum confg fg = con (p)->fg;
  return fg == PILLAR
    || fg == BIG_PILLAR_TOP
    || fg == BIG_PILLAR_BOTTOM
    || fg == ARCH_BOTTOM;
}

bool
is_rigid_con (struct pos *p)
{
  enum confg fg = con (p)->fg;
  return is_pillar (p)
    || fg == WALL || fg == DOOR || fg == CHOPPER || fg == MIRROR
    || is_arch_top (p) || is_carpet (p);
}

bool
is_carpet_cs (enum confg t)
{
  return t == CARPET || t == TCARPET;
}

bool
is_carpet (struct pos *p)
{
  enum confg t = con (p)->fg;
  return is_carpet_cs (t);
}

bool
is_arch_top (struct pos *p)
{
  return arch_top_cs (con (p)->fg);
}

bool
arch_top_cs (enum confg t)
{
  return t == ARCH_TOP_MID
    || t == ARCH_TOP_SMALL
    || t == ARCH_TOP_LEFT
    || t == ARCH_TOP_RIGHT;
}

bool
is_door (struct pos *p)
{
  enum confg t = con (p)->fg;
  return t == DOOR
    || t == LEVEL_DOOR;
}

bool
is_floor (struct pos *p)
{
  enum confg t = con (p)->fg;
  return t == FLOOR
    || t == BROKEN_FLOOR
    || t == SKELETON_FLOOR
    || t == LOOSE_FLOOR
    || t == SPIKES_FLOOR
    || t == OPENER_FLOOR
    || t == CLOSER_FLOOR
    || t == STUCK_FLOOR
    || t == HIDDEN_FLOOR;
}

bool
is_floor_like (struct pos *p)
{
  enum confg t = con (p)->fg;
  return is_floor (p)
    || t == PILLAR
    || t == BIG_PILLAR_BOTTOM
    || t == DOOR
    || t == LEVEL_DOOR
    || t == CHOPPER
    || t == MIRROR
    || t == CARPET
    || t == ARCH_BOTTOM;
}

bool
is_item_fg (struct pos *p)
{
  enum confg t = con (p)->fg;
  return t == FLOOR
    || t == BROKEN_FLOOR
    || t == SKELETON_FLOOR
    || t == STUCK_FLOOR
    || t == HIDDEN_FLOOR
    || t == PILLAR
    || t == BIG_PILLAR_BOTTOM
    || t == ARCH_BOTTOM;
}

bool
is_potion_ext (struct pos *p)
{
  enum item t = con (p)->ext.item;
  return t == EMPTY_POTION
    || t == SMALL_LIFE_POTION
    || t == BIG_LIFE_POTION
    || t == SMALL_POISON_POTION
    || t == BIG_POISON_POTION
    || t == FLOAT_POTION
    || t == FLIP_POTION
    || t == ACTIVATION_POTION;
}

bool
is_potion (struct pos *p)
{
  return is_item_fg (p) && is_potion_ext (p);
}

bool
is_sword (struct pos *p)
{
  return is_item_fg (p) && con (p)->ext.item == SWORD;
}

struct pos *
first_confg (struct pos *p0, struct pos *p1, confg_set cs, struct pos *p)
{
  struct pos np0, np1;

  npos (p0, &np0);
  pos2room (p1, np0.room, &np1);

  if (np0.room != np1.room
      || np0.floor != np1.floor) {
    invalid_pos (p);
    return p;
  }

  int inc = np0.place < np1.place ? +1 : -1;

  for (*p = np0; p->place != np1.place; prel (p, p, +0, inc))
    if (cs (con (p)->fg)) return p;

  if (cs (con (&np1)->fg)) {
    *p = np1;
    return p;
  }

  invalid_pos (p);
  return p;
}

struct pos *
next_pos_by_pred (struct pos *p, int dir, pos_pred pred, void *data)
{
  struct pos q = *p;

  if (q.room < 0 || q.floor < 0 || q.place < 0) new_pos (&q, q.l, 0, 0, -1);
  if (q.room > ROOMS - 1 || q.floor > FLOORS - 1 || q.place > PLACES - 1)
    new_pos (&q, q.l, ROOMS - 1, FLOORS - 1, PLACES);

  if (dir < 0) {
    goto loop_prev;

    for (q.room = ROOMS - 1; q.room >= 0; q.room--)
      for (q.floor = FLOORS - 1; q.floor >= 0; q.floor--)
        for (q.place = PLACES - 1; q.place >= 0; q.place--) {
          if (pred (&q, data)) {
            *p = q;
            return p;
          }

        loop_prev:
          q = q;
        }
  } else {
    goto loop_next;

    for (q.room = 0; q.room < ROOMS; q.room++)
      for (q.floor = 0; q.floor < FLOORS; q.floor++)
        for (q.place = 0; q.place < PLACES; q.place++) {
          if (pred (&q, data)) {
            *p = q;
            return p;
          }

        loop_next:
          q = q;
        }
  }

  return NULL;
}

bool
is_event_at_pos (struct pos *p, void *data)
{
  int *event = (int *) data;
  enum confg t = con (p)->fg;
  int e = con (p)->ext.event;

  return (t == OPENER_FLOOR || t == CLOSER_FLOOR)
    && (e == *event || e == -*event - 1);
}

int
next_int_by_pred (int *n, int dir, int min, int max,
                  int_pred pred, void *data)
{
  int m = *n;

  if (m < min) m = min - 1;
  if (m > max) m = max + 1;

  if (dir < 0) {
    goto loop_prev;

    for (m = max; m >= min; m--) {
      if (pred (m, data)) {
        *n = m;
        return m;
      }

    loop_prev:
      m = m;
    }
  } else {
    goto loop_next;

    for (m = min; m <= max; m++) {
      if (pred (m, data)) {
        *n = m;
        return m;
      }

    loop_next:
      m = m;
    }
  }

  return -1;
}

bool
is_pos_at_event (int e, void *_p)
{
  struct pos *p = (struct pos *) _p;
  return peq (&p->l->event[e].p, p);
}

void
exchange_event_pos (struct pos *p0, struct pos *p1)
{
  if (peq (p0, p1)) return;
  int i;
  for (i = 0; i < EVENTS; i++)
    if (peq (&p0->l->event[i].p, p0))
      p1->l->event[i].p = *p1;
    else if (peq (&p1->l->event[i].p, p1))
      p0->l->event[i].p = *p0;
}

void
exchange_guard_pos (struct pos *p0, struct pos *p1, bool invert_dir)
{
  if (peq (p0, p1)) return;
  int i;
  for (i = 0; i < GUARDS; i++)
    if (peq (&p0->l->guard[i].p, p0)) {
      p1->l->guard[i].p = *p1;
      if (invert_dir)
        p1->l->guard[i].dir = (p1->l->guard[i].dir == LEFT) ? RIGHT : LEFT;
    } else if (peq (&p1->l->guard[i].p, p1)) {
      p0->l->guard[i].p = *p0;
      if (invert_dir)
        p0->l->guard[i].dir = (p0->l->guard[i].dir == LEFT) ? RIGHT : LEFT;
    }
}

void
exchange_kid_start_pos (struct pos *p0, struct pos *p1, bool invert_dir)
{
  if (peq (p0, p1)) return;
  if (peq (&p0->l->start_pos, p0)) {
    p1->l->start_pos = *p1;
    if (invert_dir)
      p1->l->start_dir = p1->l->start_dir == LEFT ? RIGHT : LEFT;
  } else if (peq (&p1->l->start_pos, p1)) {
    p0->l->start_pos = *p0;
    if (invert_dir)
      p0->l->start_dir = p0->l->start_dir == LEFT ? RIGHT : LEFT;
  }
}

void
exchange_anim_pos (struct pos *p0, struct pos *p1, bool invert_dir)
{
  if (peq (p0, p1)) return;
  int i, j;
  for (i = 0; i < anima_nmemb; i++) {
    struct anim *a = &anima[i];

    struct pos p;
    survey (_m, pos, &a->f, NULL, &p, NULL);

    if (peq (&p, p0)) {
      place_at_pos (&a->f, _m, p1, &a->f.c);
      place_on_the_ground (&a->f, &a->f.c);
      for (j = 0; j < 2; j++) {
        if (is_valid_pos (&a->df_pos[j]))
          get_new_rel_pos (&p, &a->df_pos[j], p1, &a->df_pos[j]);
        if (is_valid_pos (&a->df_posb[j]))
          get_new_rel_pos (&p, &a->df_posb[j], p1, &a->df_posb[j]);
      }
      if (invert_dir) invert_frame_dir (&a->f, &a->f);
    } else if (peq (&p, p1)) {
      place_at_pos (&a->f, _m, p0, &a->f.c);
      place_on_the_ground (&a->f, &a->f.c);
      for (j = 0; j < 2; j++) {
        if (is_valid_pos (&a->df_pos[j]))
          get_new_rel_pos (&p, &a->df_pos[j], p0, &a->df_pos[j]);
        if (is_valid_pos (&a->df_posb[j]))
          get_new_rel_pos (&p, &a->df_posb[j], p0, &a->df_posb[j]);
      }
      if (invert_dir) invert_frame_dir (&a->f, &a->f);
    }
  }
}

void
mirror_pos (struct pos *p0, struct pos *p1, bool destroy, bool register_con,
            bool prepare, bool register_change, bool invert_dir)
{
  if (peq (p0, p1)) return;
  struct con con0 = *con (p0);
  struct con con1 = *con (p1);
  if (destroy) {
    destroy_con_at_pos (p0);
    destroy_con_at_pos (p1);
  }
  *con (p0) = con1;
  *con (p1) = con0;
  if (register_con) {
    register_con_at_pos (p0);
    register_con_at_pos (p1);
  }
  if (prepare) {
    prepare_con_at_pos (p0);
    prepare_con_at_pos (p1);
  }
  exchange_event_pos (p0, p1);
  exchange_guard_pos (p0, p1, invert_dir);
  exchange_kid_start_pos (p0, p1, invert_dir);
  exchange_anim_pos (p0, p1, invert_dir);

  if (register_change) {
    register_changed_pos (p0, -1);
    register_changed_pos (p1, -1);
  }
}

void
decorate_pos (struct pos *p)
{
  struct con *c = con (p);
  struct pos np;

  switch (c->fg) {
  case FLOOR: case BROKEN_FLOOR: case SKELETON_FLOOR:
    if (c->fg == FLOOR && c->ext.item != NO_ITEM) break;
    switch (prandom (2)) {
    case 0: c->fg = FLOOR; break;
    case 1: c->fg = BROKEN_FLOOR; break;
    case 2:
      if (is_traversable (prel (p, &np, +0, -1))) break;
      c->fg = SKELETON_FLOOR;
      break;
    }
    break;
  case PILLAR: case BIG_PILLAR_BOTTOM: case ARCH_BOTTOM:
    if (c->fg == BIG_PILLAR_BOTTOM
        && crel (p, -1, +0)->fg == BIG_PILLAR_TOP) break;
    if (c->fg == ARCH_BOTTOM
        && crel (p, -1, +0)->fg == ARCH_TOP_MID) break;
    switch (prandom (2)) {
    case 0: c->fg = PILLAR; break;
    case 1:
      if (crel (p, -1, +1)->fg == NO_FLOOR) break;
      c->fg = BIG_PILLAR_BOTTOM; break;
    case 2: c->fg = ARCH_BOTTOM; break;
    }
    break;
  default: break;
  }

  if (c->bg != WINDOW
      && c->bg != BALCONY) {
    if (is_floor (p)
        || c->fg == NO_FLOOR
        || c->fg == CHOPPER
        || c->fg == MIRROR
        || c->fg == ARCH_BOTTOM) {
      switch (prandom (4)) {
      case 0: c->bg = NO_BG; break;
      case 1: c->bg = BRICKS_01; break;
      case 2: c->bg = BRICKS_02; break;
      case 3: c->bg = BRICKS_03; break;
      case 4: c->bg = TORCH; break;
      }
    } else if (c->fg == DOOR) {
      switch (prandom (4)) {
      case 0: c->bg = NO_BG; break;
      case 1: c->bg = BRICKS_01; break;
      case 2: c->bg = BRICKS_02; break;
      case 3: c->bg = BRICKS_03; break;
      }
    } else if (c->fg == WALL
               && (wall_correlation (p) == SWS
                   || wall_correlation (p) == WWS))
      c->bg = NO_BG;
    else c->bg = NO_BRICKS;
  }

  if (is_carpet (p)) {
    switch (c->ext.design) {
    case CARPET_00:
    case CARPET_01:
      c->ext.design = prandom (1) ? CARPET_00 : CARPET_01;
      c->bg = NO_BG;
      break;
    case ARCH_CARPET_RIGHT_00:
    case ARCH_CARPET_RIGHT_01:
      c->ext.design = prandom (1)
        ? ARCH_CARPET_RIGHT_00 : ARCH_CARPET_RIGHT_01;
      c->bg = NO_BG;
      break;
    default: break;
    }
  }
}

enum con_diff
con_diff (struct con *c0, struct con *c1)
{
  if (c0->fg == c1->fg && c0->bg == c1->bg && c0->ext.step == c1->ext.step)
    return CON_DIFF_NO_DIFF;

  if (c0->fg != c1->fg && c0->bg == c1->bg && c0->ext.step == c1->ext.step)
    return CON_DIFF_FG;

  if (c0->fg == c1->fg && c0->bg != c1->bg && c0->ext.step == c1->ext.step)
    return CON_DIFF_BG;

  if (c0->fg == c1->fg && c0->bg == c1->bg && c0->ext.step != c1->ext.step)
    return CON_DIFF_EXT;

  return CON_DIFF_MIXED;
}

struct level *
mirror_room_h (struct level *l, int room, bool destroy,
               bool register_con, bool prepare, bool register_change)
{
  struct pos p0, p1;
  new_pos (&p0, l, room, -1, -1);
  for (p0.floor = 0; p0.floor < FLOORS; p0.floor++)
    for (p0.place = 0; p0.place < PLACES / 2; p0.place++) {
      reflect_pos_h (&p0, &p1);
      mirror_pos (&p0, &p1, destroy, register_con, prepare,
                  register_change, true);
    }
  return l;
}

struct level *
mirror_level_h (struct level *l, bool destroy, bool register_con,
                bool prepare, bool register_change)
{
  int i;
  for (i = 1; i < ROOMS; i++) {
    mirror_room_h (l, i, destroy, register_con, prepare, register_change);
    mirror_link (l, i, LEFT, RIGHT);
  }
  return l;
}



int
dist_next_place (struct frame *f, coord_f cf, pos_f pf,
                 int margin, bool reverse)
{
  if (cutscene) return PLACE_WIDTH + 1;

  struct frame _f = *f;

  _f.c.x += (_f.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int inc = (_f.dir == LEFT) ? r * -1 : r * +1;

  int i = 0;

  struct pos np, _np;
  survey (cf, pf, &_f, NULL, NULL, &np);

  do {
    i += inc;
    _f.c.x += inc;
    nframe (&_f, &_f.c);
    survey (cf, pf, &_f, NULL, NULL, &_np);
  } while (np.place == _np.place
           && abs (i) < PLACE_WIDTH);

  return abs (i);
}

bool
is_colliding (struct frame *f, struct frame_offset *fo, int dx,
              int reverse, struct collision_info *ci)
{
  return ! cutscene
    && (is_colliding_cf (f, fo, dx, reverse, ci, _bf)
        || is_colliding_cf (f, fo, dx, reverse, ci, _tf));
}

bool
is_colliding_cf (struct frame *f, struct frame_offset *fo, int dx,
                 int reverse, struct collision_info *ci,
                 coord_f cf)
{
  struct coord tf; struct pos pcf, _pcf, pocf, p, pl, pr;

  struct frame _f = *f, nf;

  nframe (&_f, &_f.c);

  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  next_frame_inv = (reverse == true);
  next_frame (&_f, &nf, fo);
  next_frame_inv = false;

  int dir = (nf.dir == LEFT) ? -1 : +1;
  nf.c.x += dir * dx;

  coord_f ocf = opposite_cf (cf);

  survey (cf, pos, &_f, NULL, &_pcf, NULL);
  survey (cf, pos, &nf, NULL, &pcf, NULL);
  survey (ocf, pos, &nf, NULL, &pocf, NULL);
  _tf (&nf, &tf);

  if (pcf.room != _pcf.room) pos2room (&pcf, _f.c.room, &pcf);
  if (pocf.room != pcf.room) pos2room (&pocf, pcf.room, &pocf);

  bool wall_collision = false;
  bool door_collision = false;
  bool carpet_collision = false;
  bool mirror_collision = false;

  /* printf ("_pcf: (%i,%i,%i); pcf: (%i,%i,%i)\n", */
  /*         _pcf.room, _pcf.floor, _pcf.place, */
  /*         pcf.room, pcf.floor, pcf.place); */

  /* wall */

  if (_f.dir == LEFT && pcf.place <= _pcf.place
      && _f.c.room != roomd (_f.c.l, _f.c.room, LEFT))
    for (p = _pcf; p.place >= pcf.place; prel (&p, &p, +0, -1))
      if (con (&p)->fg == WALL) {
        wall_collision = true;
        ci->p = p;
        break;
      }

  if (_f.dir == RIGHT && pcf.place >= _pcf.place
      && _f.c.room != roomd (_f.c.l, _f.c.room, RIGHT))
    for (p = _pcf; p.place <= pcf.place; prel (&p, &p, +0, +1))
      if (con (&p)->fg == WALL) {
        wall_collision = true;
        ci->p = p;
        break;
      }

  if (con (&pcf)->fg == WALL) {
    wall_collision = true;
    ci->p = pcf;
  }


  /* door */

  if (_f.dir == LEFT && pcf.place < _pcf.place
      && _f.c.room != roomd (_f.c.l, _f.c.room, LEFT))
    for (prel (&_pcf, &p, +0, -1); p.place >= pcf.place; prel (&p, &p, +0, -1))
      if (con (&p)->fg == DOOR
          && tf.y <= door_grid_tip_y (&p) - 10) {
        door_collision = true;
        ci->p = p;
        break;
      }

  if (_f.dir == RIGHT && pcf.place > _pcf.place
      && _f.c.room != roomd (_f.c.l, _f.c.room, RIGHT))
    for (prel (&_pcf, &p, +0, +1); p.place <= pcf.place; prel (&p, &p, +0, +1)) {
      prel (&p, &pl, +0, -1);
      if (con (&pl)->fg == DOOR
          && tf.y <= door_grid_tip_y (&pl) - 10) {
        door_collision = true;
        ci->p = p;
        break;
      }
    }

  if (con (&pcf)->fg == DOOR
      && pcf.place < pocf.place
      && tf.y <= door_grid_tip_y (&pcf) - 10) {
    door_collision = true;
    ci->p = pcf;
  }

  if (con (&pocf)->fg == DOOR
      && pocf.place < pcf.place
      && tf.y <= door_grid_tip_y (&pocf) - 10) {
    door_collision = true;
    prel (&pocf, &ci->p, +0, +1);
  }


  /* carpet */

  if (_f.dir == LEFT && pcf.place < _pcf.place
      && _f.c.room != roomd (_f.c.l, _f.c.room, LEFT))
    for (prel (&_pcf, &p, +0, -1); p.place >= pcf.place; prel (&p, &p, +0, -1))
      if (is_carpet (&p)) {
        carpet_collision = true;
        ci->p = p;
        break;
      }

  if (_f.dir == RIGHT && pcf.place > _pcf.place
      && _f.c.room != roomd (_f.c.l, _f.c.room, RIGHT))
    for (prel (&_pcf, &p, +0, +1); p.place <= pcf.place; prel (&p, &p, +0, +1)) {
      prel (&p, &pl, +0, -1);
      if (is_carpet (&pl)) {
        carpet_collision = true;
        ci->p = p;
        break;
      }
    }

  if (is_carpet (&pcf)
      && pcf.place < pocf.place) {
    carpet_collision = true;
    ci->p = pcf;
  }

  if (is_carpet (&pocf)
      && pocf.place < pcf.place) {
    carpet_collision = true;
    prel (&pocf, &ci->p, +0, +1);
  }


  /* mirror */

  if (_f.dir == LEFT && pcf.place < _pcf.place
      && _f.c.room != roomd (_f.c.l, _f.c.room, LEFT))
    for (prel (&_pcf, &p, +0, -1); p.place >= pcf.place; prel (&p, &p, +0, -1)) {
      prel (&p, &pr, +0, +1);
      if (con (&pr)->fg == MIRROR) {
        mirror_collision = true;
        ci->p = p;
        break;
      }
    }

  if (_f.dir == RIGHT && pcf.place > _pcf.place
      && _f.c.room != roomd (_f.c.l, _f.c.room, RIGHT))
    for (prel (&_pcf, &p, +0, +1); p.place <= pcf.place; prel (&p, &p, +0, +1)) {
      if (con (&p)->fg == MIRROR) {
        mirror_collision = true;
        ci->p = p;
        break;
      }
    }

  if (con (&pocf)->fg == MIRROR
      && pcf.place < pocf.place) {
    mirror_collision = true;
    prel (&pocf, &ci->p, +0, -1);
  }

  if (con (&pcf)->fg == MIRROR
      && pocf.place < pcf.place) {
    mirror_collision = true;
    ci->p = pcf;
  }

  ci->t = NO_FLOOR;
  if (wall_collision) ci->t = WALL;
  if (door_collision) ci->t = DOOR;
  if (carpet_collision) ci->t = CARPET;
  if (mirror_collision) ci->t = MIRROR;

  /* if (wall_collision || door_collision || carpet_collision || mirror_collision) */
  /*   pos2room (&ci->p, _f.c.room, &ci->p); */

  /* if (door_collision) printf ("DOOR COLLISION!\n"); */
  /* if (wall_collision) printf ("WALL COLLISION!\n"); */
  /* if (carpet_collision) printf ("CARPET COLLISION!\n"); */
  /* if (mirror_collision) printf ("MIRROR COLLISION!\n"); */

  return wall_collision || door_collision || carpet_collision || mirror_collision;
}

struct frame_offset *
uncollide (struct frame *f, struct frame_offset *fo,
           struct frame_offset *_fo, int dx,
           int reverse, struct collision_info *ci)
{
  struct frame_offset fo0 = *fo;
  struct frame_offset fo1 = *fo;

  while (is_colliding (f, &fo0, dx, reverse, ci)
         && abs (fo0.dx) <= PLACE_WIDTH)
    fo0.dx++;

  while (is_colliding (f, &fo1, dx, reverse, ci)
         && abs (fo1.dx) <= PLACE_WIDTH)
    fo1.dx--;

  *_fo = (abs (fo0.dx) < abs (fo1.dx)) ? fo0 : fo1;

  return _fo;
}

bool
is_on_con (struct frame *f, coord_f cf, pos_f pf,
           int margin, bool reverse, int min_dist, enum confg t)
{
  struct frame _f = *f;
  int dn = dist_next_place (&_f, cf, pf, margin, reverse);

  _f.c.x += (_f.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int dir = (_f.dir == LEFT) ? r * -1: r * +1;

  struct pos p;
  survey (cf, pf, &_f, NULL, &p, NULL);

  return dn <= min_dist && crel (&p, 0, dir)->fg == t;
}

int
dist_collision (struct frame *f, int reverse,
                struct collision_info *ci)
{
  if (cutscene) return PLACE_WIDTH + 1;

  int i = 0, dx = +1;
  struct frame _f = *f;
  struct frame_offset _fo;

  _fo.b = _f.b;
  _fo.dx = _fo.dy = 0;

  int r = (reverse) ? -1 : + 1;
  int dir = (_f.dir == LEFT) ? -1 : +1;

  if (! is_colliding (&_f, &_fo, dx, reverse, ci))
    while (! is_colliding (&_f, &_fo, dx, reverse, ci)
           && i < PLACE_WIDTH + 1) {
      _f.c.x += r * dir;
      i++;
    }
  else
    while (is_colliding (&_f, &_fo, dx, reverse, ci)
           && -i < PLACE_WIDTH + 1) {
      _f.c.x -= r * dir;
      i--;
    }

  return i;
}

int
dist_con (struct frame *f, coord_f cf, pos_f pf,
          int margin, bool reverse, enum confg t)
{
  if (cutscene) return PLACE_WIDTH + 1;

  int dn = dist_next_place (f, cf, pf, margin, reverse);
  if (is_on_con (f, cf, pf, margin, reverse, dn, t)) return dn;
  else return PLACE_WIDTH + 1;
}

int
dist_chopper (struct frame *f, bool reverse)
{
  if (cutscene) return PLACE_WIDTH + 1;

  struct pos ptf, ptfr;

  struct frame _f = *f;
  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  enum confg ctf = survey (_tf, pos, &_f, NULL, &ptf, NULL)->fg;
  prel (&ptf, &ptfr, +0, +1);
  enum confg ctfr = con (&ptfr)->fg;

  if (_f.dir == LEFT && ctf == CHOPPER)
    return dist_next_place (&_f, _tf, pos, +0, false);
  else if (_f.dir == RIGHT && ctfr == CHOPPER)
    return dist_next_place (&_f, _tf, pos, +0, false);

  return PLACE_WIDTH + 1;
}

int
dist_fall (struct frame *f, bool reverse)
{
  if (cutscene) return PLACE_WIDTH + 1;

  int dnf, dbpt, datm, dats, datl, datr, dtc;

  dnf = dist_con (f, _bf, pos, -4, reverse, NO_FLOOR);
  dbpt = dist_con (f, _bf, pos, -4, reverse, BIG_PILLAR_TOP);
  datm = dist_con (f, _bf, pos, -4, reverse, ARCH_TOP_MID);
  dats = dist_con (f, _bf, pos, -4, reverse, ARCH_TOP_SMALL);
  datl = dist_con (f, _bf, pos, -4, reverse, ARCH_TOP_LEFT);
  datr = dist_con (f, _bf, pos, -4, reverse, ARCH_TOP_RIGHT);
  dtc = dist_con (f, _bf, pos, -4, reverse, TCARPET);

  if (dnf <= PLACE_WIDTH) return dnf;
  if (dbpt <= PLACE_WIDTH) return dbpt;
  if (datm <= PLACE_WIDTH) return datm;
  if (dats <= PLACE_WIDTH) return dats;
  if (datl <= PLACE_WIDTH) return datl;
  if (datr <= PLACE_WIDTH) return datr;
  if (dtc <= PLACE_WIDTH) return dtc;

  return PLACE_WIDTH + 1;
}



bool
is_hangable_cs (enum confg t, enum dir d)
{
  return t == FLOOR
    || t == BROKEN_FLOOR
    || t == LOOSE_FLOOR
    || t == SKELETON_FLOOR
    || t == SPIKES_FLOOR
    || t == OPENER_FLOOR
    || t == CLOSER_FLOOR
    || t == STUCK_FLOOR
    || t == HIDDEN_FLOOR
    || t == PILLAR
    || t == BIG_PILLAR_BOTTOM
    || t == DOOR
    || t == LEVEL_DOOR
    || (t == CHOPPER && d == LEFT)
    || t == ARCH_BOTTOM
    || (t == CARPET && d == RIGHT)
    || (t == MIRROR && d == LEFT);
}

bool
is_hangable_pos (struct pos *p, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;
  struct pos ph; prel (p, &ph, -1, dir);
  struct con *ch = con (&ph);
  struct pos pa; prel (p, &pa, -1, 0);
  struct pos pr; prel (p, &pr, +0, +1);
  struct con *cr = con (&pr);
  struct con *c = con (p);

  return is_hangable_cs (ch->fg, d)
    && is_strictly_traversable (&pa)
    && ! (d == LEFT && c->fg == CHOPPER)
    && ! (d == LEFT && c->fg == MIRROR)
    && ! (d == RIGHT && cr->fg == CHOPPER)
    && ! (d == RIGHT && cr->fg == MIRROR)
    && ! (d == RIGHT && is_carpet (p))
    && ! (d == RIGHT && is_carpet (&pa));
}

bool
can_hang (struct frame *f, bool reverse, struct pos *hang_pos)
{
  struct frame _f = *f;

  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  nframe (&_f, &_f.c);

  struct coord tb; _tb (&_f, &tb);

  struct pos pbf, pmbo, pbb, pbbb;
  survey (_bf, pos, &_f, NULL, &pbf, NULL);
  survey (_mbo, pos, &_f, NULL, &pmbo, NULL);
  survey (_bb, pos, &_f, NULL, &pbb, NULL);
  prel (&pbb, &pbbb, +0, (_f.dir == LEFT) ? +1 : -1);

  bool hbf = is_hangable_pos (&pbf, _f.dir);
  bool hmbo = is_hangable_pos (&pmbo, _f.dir);
  bool hbb = is_hangable_pos (&pbb, _f.dir);
  bool hbbb = is_hangable_pos (&pbbb, _f.dir);

  if (! hbf && ! hmbo && ! hbb && !hbbb)
    return false;

  if (hbbb) *hang_pos = pbbb;
  if (hbb) *hang_pos = pbb;
  if (hmbo) *hang_pos = pmbo;
  if (hbf) *hang_pos = pbf;

  pos2room (hang_pos, _f.c.room, hang_pos);

  /* for fall */
  struct coord ch;
  ch.x = PLACE_WIDTH * (hang_pos->place + 1) - 1;
  ch.y = PLACE_HEIGHT * hang_pos->floor;

  float d = dist_coord (&tb, &ch);

  /* if (! reverse) printf ("dist_coord = %f\n", d); */

  if (is_kid_fall (&_f) && d > 18) return false;

  return true;
}

struct pos *
get_hanged_pos (struct pos *hang_pos, enum dir d, struct pos *p)
{
  int dir = (d == LEFT) ? -1 : +1;
  prel (hang_pos, p, -1, dir);
  pos2room (p, hang_pos->room, p);
  return p;
}

enum confg
get_hanged_con (struct pos *hang_pos, enum dir d)
{
  struct pos p;
  return con (get_hanged_pos (hang_pos, d, &p))->fg;
}

bool
is_hang_pos_critical (struct pos *hang_pos)
{
  return (is_traversable (hang_pos));
}

bool
is_hang_pos_free (struct pos *hang_pos, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;
  struct pos ptf; prel (hang_pos, &ptf, 0, dir);
  enum confg ctf = con (&ptf)->fg;
  return ! (ctf == WALL || (ctf == DOOR && d == LEFT)
            || (is_carpet (&ptf) && d == LEFT));
}



void
update_depressible_floor (struct anim *a, int dx0, int dx1)
{
  if (cutscene) return;

  struct coord c0, c1;
  struct pos p0, p1;

  int dir = (a->f.dir == LEFT) ? -1 : 1;

  _bf (&a->f, &c0);
  c0.x += dir * dx0;
  pos (&c0, &p0);

  _bf (&a->f, &c1);
  c1.x += dir * dx1;
  pos (&c1, &p1);

  if (a->current_lives <= 0 && con (&p0)->fg == CLOSER_FLOOR)
    closer_floor_at_pos (&p0)->unresponsive = true;
  press_depressible_floor (&p0);
  a->df_pos[0] = p0;

  if (a->current_lives <= 0 && con (&p1)->fg == CLOSER_FLOOR)
    closer_floor_at_pos (&p1)->unresponsive = true;
  press_depressible_floor (&p1);
  a->df_pos[1] = p1;
}

void
keep_depressible_floor (struct anim *a)
{
  struct pos *p0 = &a->df_pos[0];
  struct pos *p1 = &a->df_pos[1];

  press_depressible_floor (p0);
  press_depressible_floor (p1);
}

void
clear_depressible_floor (struct anim *a)
{
  invalid_pos (&a->df_pos[0]);
  invalid_pos (&a->df_pos[1]);
}

void
save_depressible_floor (struct anim *a)
{
  a->df_posb[0] = a->df_pos[0];
  a->df_posb[1] = a->df_pos[1];
}

void
restore_depressible_floor (struct anim *a)
{
  a->df_pos[0] = a->df_posb[0];
  a->df_pos[1] = a->df_posb[1];
}

void
press_depressible_floor (struct pos *p)
{
  if (cutscene || ! is_valid_pos (p)) return;

  switch (con (p)->fg) {
  case OPENER_FLOOR: press_opener_floor (p); break;
  case CLOSER_FLOOR: press_closer_floor (p); break;
  case LOOSE_FLOOR: release_loose_floor (p); break;
  case HIDDEN_FLOOR: unhide_hidden_floor (p); break;
  default: break;
  }
}

void
unhide_hidden_floor (struct pos *p)
{
  if (con (p)->fg != HIDDEN_FLOOR) return;
  else {
    register_con_undo
      (&undo, p,
       FLOOR, MIGNORE, MIGNORE,
       false, false, false, false,
       CHPOS_UNHIDE_FLOOR,
       "UNHIDE FLOOR");
  }
}




void
activate_con (struct pos *p)
{
  struct door *d;
  struct closer_floor *c;
  struct opener_floor *o;

  switch (con (p)->fg) {
  case SPIKES_FLOOR:
    spikes_floor_at_pos (p)->activate = true; break;
  case DOOR:
    d = door_at_pos (p);
    d->action = (d->i) ? OPEN_DOOR : CLOSE_DOOR;
    break;
  case OPENER_FLOOR:
    o = opener_floor_at_pos (p);
    o->noise = true;
    press_opener_floor (p); break;
  case CLOSER_FLOOR:
    c = closer_floor_at_pos (p);
    c->noise = true;
    press_closer_floor (p); break;
  case HIDDEN_FLOOR: unhide_hidden_floor (p); break;
  case LOOSE_FLOOR: release_loose_floor (p); break;
  case CHOPPER: chopper_at_pos (p)->activate = true; break;
  case LEVEL_DOOR: level_door_at_pos (p)->action = OPEN_LEVEL_DOOR;
    break;
  default: break;
  }
}

char *
get_confg_name (enum confg t)
{
  switch (t) {
  case NO_FLOOR: return "NO FLOOR";
  case FLOOR: return "FLOOR";
  case BROKEN_FLOOR: return "BROKEN FLOOR";
  case SKELETON_FLOOR: return "SKELETON FLOOR";
  case LOOSE_FLOOR: return "LOOSE FLOOR";
  case SPIKES_FLOOR: return "SPIKES FLOOR";
  case OPENER_FLOOR: return "OPENER FLOOR";
  case CLOSER_FLOOR: return "CLOSER FLOOR";
  case STUCK_FLOOR: return "STUCK FLOOR";
  case HIDDEN_FLOOR: return "HIDDEN FLOOR";
  case PILLAR: return "PILLAR";
  case BIG_PILLAR_BOTTOM: return "BIG PILLAR BOTTOM";
  case BIG_PILLAR_TOP: return "BIG PILLAR TOP";
  case WALL: return "WALL";
  case DOOR: return "DOOR";
  case LEVEL_DOOR: return "LEVEL DOOR";
  case CHOPPER: return "CHOPPER";
  case MIRROR: return "MIRROR";
  case CARPET: return "CARPET";
  case TCARPET: return "TCARPET";
  case ARCH_BOTTOM: return "ARCH BOTTOM";
  case ARCH_TOP_LEFT: return "ARCH TOP LEFT";
  case ARCH_TOP_RIGHT: return "ARCH TOP RIGHT";
  case ARCH_TOP_MID: return "ARCH TOP MID";
  case ARCH_TOP_SMALL: return "ARCH TOP SMALL";
  default: return "UNKNOWN FG";
  }
}

char *
get_conbg_name (enum conbg t)
{
  switch (t) {
  case NO_BG: return "NO BG";
  case BRICKS_00: return "BRICKS 00";
  case BRICKS_01: return "BRICKS 01";
  case BRICKS_02: return "BRICKS 02";
  case BRICKS_03: return "BRICKS 03";
  case NO_BRICKS: return "NO BRICKS";
  case TORCH: return "TORCH";
  case WINDOW: return "WINDOW";
  case BALCONY: return "BALCONY";
  default: return "UNKNOWN BG";
  }
}

char *
get_item_name (enum item t)
{
  switch (t) {
  case NO_ITEM: return "NO ITEM";
  case EMPTY_POTION: return "EMPTY POTION";
  case SMALL_LIFE_POTION: return "SMALL LIFE POTION";
  case BIG_LIFE_POTION: return "BIG LIFE POTION";
  case SMALL_POISON_POTION: return "SMALL POISON POTION";
  case BIG_POISON_POTION: return "BIG POISON POTION";
  case FLOAT_POTION: return "FLOAT POTION";
  case FLIP_POTION: return "FLIP POTION";
  case ACTIVATION_POTION: return "ACTIVATION POTION";
  case SWORD: return "SWORD";
  default: return "UNKNOWN EXTENSION";
  }
}
