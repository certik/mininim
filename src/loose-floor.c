/*
  loose-floor.c -- loose floor module;

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
ALLEGRO_BITMAP *dc_loose_floor_left_00, *dc_loose_floor_right_00,
  *dc_loose_floor_base_00, *dc_loose_floor_left_01, *dc_loose_floor_right_01,
  *dc_loose_floor_base_01, *dc_loose_floor_01, *dc_broken_floor;

/* palace cga */
ALLEGRO_BITMAP *pc_loose_floor_left_00, *pc_loose_floor_right_00,
  *pc_loose_floor_base_00, *pc_loose_floor_left_01, *pc_loose_floor_right_01,
  *pc_loose_floor_base_01, *pc_loose_floor_01, *pc_broken_floor;

/* dungeon ega */
ALLEGRO_BITMAP *de_loose_floor_left_00, *de_loose_floor_right_00,
  *de_loose_floor_base_00, *de_loose_floor_left_01, *de_loose_floor_right_01,
  *de_loose_floor_base_01, *de_loose_floor_01, *de_broken_floor;

/* palace ega */
ALLEGRO_BITMAP *pe_loose_floor_left_00, *pe_loose_floor_right_00,
  *pe_loose_floor_base_00, *pe_loose_floor_left_01, *pe_loose_floor_right_01,
  *pe_loose_floor_base_01, *pe_loose_floor_01, *pe_broken_floor;

/* dungeon vga */
ALLEGRO_BITMAP *dv_loose_floor_left_00, *dv_loose_floor_right_00,
  *dv_loose_floor_base_00, *dv_loose_floor_left_01, *dv_loose_floor_right_01,
  *dv_loose_floor_base_01, *dv_loose_floor_01, *dv_broken_floor;

/* palace vga */
ALLEGRO_BITMAP *pv_loose_floor_left_00, *pv_loose_floor_right_00,
  *pv_loose_floor_base_00, *pv_loose_floor_left_01, *pv_loose_floor_right_01,
  *pv_loose_floor_base_01, *pv_loose_floor_01, *pv_broken_floor;

struct loose_floor *loose_floor = NULL;
size_t loose_floor_nmemb = 0;
static bool must_sort;

void
load_loose_floor (void)
{
  /* dungeon cga */
  dc_loose_floor_left_00 = load_bitmap (DC_LOOSE_FLOOR_LEFT_00);
  dc_loose_floor_right_00 = load_bitmap (DC_LOOSE_FLOOR_RIGHT_00);
  dc_loose_floor_base_00 = load_bitmap (DC_LOOSE_FLOOR_BASE_00);
  dc_loose_floor_left_01 = load_bitmap (DC_LOOSE_FLOOR_LEFT_01);
  dc_loose_floor_right_01 = load_bitmap (DC_LOOSE_FLOOR_RIGHT_01);
  dc_loose_floor_base_01 = load_bitmap (DC_LOOSE_FLOOR_BASE_01);
  dc_loose_floor_01 = create_loose_floor_01_bitmap (DUNGEON, CGA);
  dc_broken_floor = create_broken_floor_bitmap (DUNGEON, CGA);

  /* palace cga */
  pc_loose_floor_left_00 = load_bitmap (PC_LOOSE_FLOOR_LEFT_00);
  pc_loose_floor_right_00 = load_bitmap (PC_LOOSE_FLOOR_RIGHT_00);
  pc_loose_floor_base_00 = load_bitmap (PC_LOOSE_FLOOR_BASE_00);
  pc_loose_floor_left_01 = load_bitmap (PC_LOOSE_FLOOR_LEFT_01);
  pc_loose_floor_right_01 = load_bitmap (PC_LOOSE_FLOOR_RIGHT_01);
  pc_loose_floor_base_01 = load_bitmap (PC_LOOSE_FLOOR_BASE_01);
  pc_loose_floor_01 = create_loose_floor_01_bitmap (PALACE, CGA);
  pc_broken_floor = create_broken_floor_bitmap (PALACE, CGA);

  /* dungeon ega */
  de_loose_floor_left_00 = load_bitmap (DE_LOOSE_FLOOR_LEFT_00);
  de_loose_floor_right_00 = load_bitmap (DE_LOOSE_FLOOR_RIGHT_00);
  de_loose_floor_base_00 = load_bitmap (DE_LOOSE_FLOOR_BASE_00);
  de_loose_floor_left_01 = load_bitmap (DE_LOOSE_FLOOR_LEFT_01);
  de_loose_floor_right_01 = load_bitmap (DE_LOOSE_FLOOR_RIGHT_01);
  de_loose_floor_base_01 = load_bitmap (DE_LOOSE_FLOOR_BASE_01);
  de_loose_floor_01 = create_loose_floor_01_bitmap (DUNGEON, EGA);
  de_broken_floor = create_broken_floor_bitmap (DUNGEON, EGA);

  /* palace ega */
  pe_loose_floor_left_00 = load_bitmap (PE_LOOSE_FLOOR_LEFT_00);
  pe_loose_floor_right_00 = load_bitmap (PE_LOOSE_FLOOR_RIGHT_00);
  pe_loose_floor_base_00 = load_bitmap (PE_LOOSE_FLOOR_BASE_00);
  pe_loose_floor_left_01 = load_bitmap (PE_LOOSE_FLOOR_LEFT_01);
  pe_loose_floor_right_01 = load_bitmap (PE_LOOSE_FLOOR_RIGHT_01);
  pe_loose_floor_base_01 = load_bitmap (PE_LOOSE_FLOOR_BASE_01);
  pe_loose_floor_01 = create_loose_floor_01_bitmap (PALACE, EGA);
  pe_broken_floor = create_broken_floor_bitmap (PALACE, EGA);

  /* dungeon vga */
  dv_loose_floor_left_00 = load_bitmap (DV_LOOSE_FLOOR_LEFT_00);
  dv_loose_floor_right_00 = load_bitmap (DV_LOOSE_FLOOR_RIGHT_00);
  dv_loose_floor_base_00 = load_bitmap (DV_LOOSE_FLOOR_BASE_00);
  dv_loose_floor_left_01 = load_bitmap (DV_LOOSE_FLOOR_LEFT_01);
  dv_loose_floor_right_01 = load_bitmap (DV_LOOSE_FLOOR_RIGHT_01);
  dv_loose_floor_base_01 = load_bitmap (DV_LOOSE_FLOOR_BASE_01);
  dv_loose_floor_01 = create_loose_floor_01_bitmap (DUNGEON, VGA);
  dv_broken_floor = create_broken_floor_bitmap (DUNGEON, VGA);

  /* palace vga */
  pv_loose_floor_left_00 = load_bitmap (PV_LOOSE_FLOOR_LEFT_00);
  pv_loose_floor_right_00 = load_bitmap (PV_LOOSE_FLOOR_RIGHT_00);
  pv_loose_floor_base_00 = load_bitmap (PV_LOOSE_FLOOR_BASE_00);
  pv_loose_floor_left_01 = load_bitmap (PV_LOOSE_FLOOR_LEFT_01);
  pv_loose_floor_right_01 = load_bitmap (PV_LOOSE_FLOOR_RIGHT_01);
  pv_loose_floor_base_01 = load_bitmap (PV_LOOSE_FLOOR_BASE_01);
  pv_loose_floor_01 = create_loose_floor_01_bitmap (PALACE, VGA);
  pv_broken_floor = create_broken_floor_bitmap (PALACE, VGA);
}

void
unload_loose_floor (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_loose_floor_left_00);
  destroy_bitmap (dc_loose_floor_right_00);
  destroy_bitmap (dc_loose_floor_base_00);
  destroy_bitmap (dc_loose_floor_left_01);
  destroy_bitmap (dc_loose_floor_right_01);
  destroy_bitmap (dc_loose_floor_base_01);
  destroy_bitmap (dc_loose_floor_01);
  destroy_bitmap (dc_broken_floor);

  /* palace cga */
  destroy_bitmap (pc_loose_floor_left_00);
  destroy_bitmap (pc_loose_floor_right_00);
  destroy_bitmap (pc_loose_floor_base_00);
  destroy_bitmap (pc_loose_floor_left_01);
  destroy_bitmap (pc_loose_floor_right_01);
  destroy_bitmap (pc_loose_floor_base_01);
  destroy_bitmap (pc_loose_floor_01);
  destroy_bitmap (pc_broken_floor);

  /* dungeon ega */
  destroy_bitmap (de_loose_floor_left_00);
  destroy_bitmap (de_loose_floor_right_00);
  destroy_bitmap (de_loose_floor_base_00);
  destroy_bitmap (de_loose_floor_left_01);
  destroy_bitmap (de_loose_floor_right_01);
  destroy_bitmap (de_loose_floor_base_01);
  destroy_bitmap (de_loose_floor_01);
  destroy_bitmap (de_broken_floor);

  /* palace ega */
  destroy_bitmap (pe_loose_floor_left_00);
  destroy_bitmap (pe_loose_floor_right_00);
  destroy_bitmap (pe_loose_floor_base_00);
  destroy_bitmap (pe_loose_floor_left_01);
  destroy_bitmap (pe_loose_floor_right_01);
  destroy_bitmap (pe_loose_floor_base_01);
  destroy_bitmap (pe_loose_floor_01);
  destroy_bitmap (pe_broken_floor);

  /* dungeon vga */
  destroy_bitmap (dv_loose_floor_left_00);
  destroy_bitmap (dv_loose_floor_right_00);
  destroy_bitmap (dv_loose_floor_base_00);
  destroy_bitmap (dv_loose_floor_left_01);
  destroy_bitmap (dv_loose_floor_right_01);
  destroy_bitmap (dv_loose_floor_base_01);
  destroy_bitmap (dv_loose_floor_01);
  destroy_bitmap (dv_broken_floor);

  /* palace vga */
  destroy_bitmap (pv_loose_floor_left_00);
  destroy_bitmap (pv_loose_floor_right_00);
  destroy_bitmap (pv_loose_floor_base_00);
  destroy_bitmap (pv_loose_floor_left_01);
  destroy_bitmap (pv_loose_floor_right_01);
  destroy_bitmap (pv_loose_floor_base_01);
  destroy_bitmap (pv_loose_floor_01);
  destroy_bitmap (pv_broken_floor);
}

ALLEGRO_BITMAP *
create_loose_floor_01_bitmap (enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *loose_floor_base_01 = NULL,
    *loose_floor_left_01 = NULL,
    *loose_floor_right_01 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      loose_floor_base_01 = dc_loose_floor_base_01;
      loose_floor_left_01 = dc_loose_floor_left_01;
      loose_floor_right_01 = dc_loose_floor_right_01;
      break;
    case EGA:
      loose_floor_base_01 = de_loose_floor_base_01;
      loose_floor_left_01 = de_loose_floor_left_01;
      loose_floor_right_01 = de_loose_floor_right_01;
      break;
    case VGA:
      loose_floor_base_01 = dv_loose_floor_base_01;
      loose_floor_left_01 = dv_loose_floor_left_01;
      loose_floor_right_01 = dv_loose_floor_right_01;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      loose_floor_base_01 = pc_loose_floor_base_01;
      loose_floor_left_01 = pc_loose_floor_left_01;
      loose_floor_right_01 = pc_loose_floor_right_01;
      break;
    case EGA:
      loose_floor_base_01 = pe_loose_floor_base_01;
      loose_floor_left_01 = pe_loose_floor_left_01;
      loose_floor_right_01 = pe_loose_floor_right_01;
      break;
    case VGA:
      loose_floor_base_01 = pv_loose_floor_base_01;
      loose_floor_left_01 = pv_loose_floor_left_01;
      loose_floor_right_01 = pv_loose_floor_right_01;
      break;
    }
    break;
  }

  int wl = al_get_bitmap_width (loose_floor_left_01);
  int wr = al_get_bitmap_width (loose_floor_right_01);
  int w = wl + wr;
  int hl = al_get_bitmap_height (loose_floor_left_01);
  int hr = al_get_bitmap_height (loose_floor_right_01);
  int hb = al_get_bitmap_height (loose_floor_base_01);
  int h = max_int (hl, hr) + hb;

  ALLEGRO_BITMAP *bitmap = create_bitmap (w, h);
  clear_bitmap (bitmap, al_map_rgba (0, 0, 0, 0));
  draw_bitmap (loose_floor_base_01, bitmap, 0, 14, 0);
  draw_bitmap (loose_floor_left_01, bitmap, 0, 1, 0);
  draw_bitmap (loose_floor_right_01, bitmap, 32, 0, 0);

  validate_bitmap_for_mingw (bitmap);

  return bitmap;
}

void
register_loose_floor (struct pos *p)
{
  assert (fg (p) == LOOSE_FLOOR
          && (loose_floor_at_pos (p) == NULL
              || loose_floor_at_pos (p)->action == FALL_LOOSE_FLOOR));

  struct loose_floor l;

  l.p = *p;
  l.i = 0;
  l.resist = LOOSE_FLOOR_RESISTENCE;
  l.action = NO_LOOSE_FLOOR_ACTION;
  l.state = 0;
  l.cant_fall = ext (p);
  l.remove = false;

  struct coord c; floor_left_coord (p, &c);
  l.f.b = get_correct_falling_loose_floor_bitmap (dv_loose_floor_01);
  l.f.flip = 0;
  new_coord (&l.f.c, p->l, p->room, c.x, c.y + 3);

  loose_floor =
    add_to_array (&l, 1, loose_floor, &loose_floor_nmemb,
                  loose_floor_nmemb, sizeof (l));

  sort_loose_floors ();
}

void
sort_loose_floors (void)
{
  qsort (loose_floor, loose_floor_nmemb, sizeof (struct loose_floor),
         compare_loose_floors);
}

int
compare_loose_floors (const void *l0, const void *l1)
{
  return cpos (&((struct loose_floor *) l0)->p,
               &((struct loose_floor *) l1)->p);
}

struct loose_floor *
loose_floor_at_pos (struct pos *p)
{
  struct loose_floor l;
  l.p = *p;

  return bsearch (&l, loose_floor, loose_floor_nmemb, sizeof (l),
                  compare_loose_floors);
}

void
remove_loose_floor (struct loose_floor *l)
{
  size_t i =  l - loose_floor;
  loose_floor =
    remove_from_array (loose_floor, &loose_floor_nmemb, i, 1, sizeof (*l));
}

void
release_loose_floor (struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (l->action != RELEASE_LOOSE_FLOOR
      && l->action != FALL_LOOSE_FLOOR
      && ! l->cant_fall) {
    l->action = RELEASE_LOOSE_FLOOR;
    l->i = 0;
  }
}

void
compute_loose_floors (void)
{
  size_t i;

  must_sort = false;

  for (i = 0; i < loose_floor_nmemb; i++) {
    struct loose_floor *l = &loose_floor[i];

    switch (l->action) {
    case SHAKE_LOOSE_FLOOR:
      compute_loose_floor_shake (l);
      break;
    case RELEASE_LOOSE_FLOOR:
      compute_loose_floor_release (l);
      break;
    case FALL_LOOSE_FLOOR: compute_loose_floor_fall (l); break;
    default: break;
    }
  }

 again:
  for (i = 0; i < loose_floor_nmemb; i++) {
    struct loose_floor *l = &loose_floor[i];
    if (l->remove) {
      must_sort = true;
      remove_loose_floor (l);
      goto again;
    }
  }

  if (must_sort) sort_loose_floors ();
}

void
compute_loose_floor_shake (struct loose_floor *l)
{
  int prev_state = l->state;

  switch (l->i) {
  case 0: l->state = 1;
    /* alert_guards (&l->p); */
    sample_random_loose_floor (&l->p); l->i++; break;
  case 1: l->state = 0; l->i++; break;
  case 2: l->state = 2;
    sample_random_loose_floor (&l->p); l->i++; break;
  case 3: l->state = 0;
    l->action = NO_LOOSE_FLOOR_ACTION; l->i = 0; break;
  }

  if (prev_state != l->state)
    register_changed_pos (&l->p, CHPOS_SHAKE_LOOSE_FLOOR);
}

void
compute_loose_floor_release (struct loose_floor *l)
{
  if (l->resist-- > 0) {
    l->state = 0;
    return;
  }

  int prev_state = l->state;

  switch (l->i) {
  case 0: l->state = 1;
    alert_guards (&l->p);
    sample_random_loose_floor (&l->p); l->i++; break;
  case 1: l->state = 0; l->i++; break;
  case 2: l->state = 2;
    sample_random_loose_floor (&l->p); l->i++; break;
  case 3: l->state = 2; l->i++; break;
  case 4: l->state = 0; l->i++; break;
  case 5: l->state = 0; l->i++; break;
  case 6: l->state = 0; l->i++; break;
  case 7: l->state = 2;
    sample_random_loose_floor (&l->p); l->i++; break;
  case 8: l->state = 2; l->i++; break;
  case 9: l->state = 2; l->i++; break;
  case 10:
    register_con_undo (&undo, &l->p,
                       NO_FLOOR, MIGNORE, MIGNORE,
                       false, false, false, false,
                       CHPOS_LOOSE_FLOOR_FALL,
                       "LOOSE FLOOR RELEASE");
    l->state = 2;
    l->i = 0;
    l->f.parent.frame = &l->f;
    l->action = FALL_LOOSE_FLOOR;
    l->resist = LOOSE_FLOOR_RESISTENCE;
    l->f.b = get_correct_falling_loose_floor_bitmap (l->f.b);
    break;
  }

  if (prev_state != l->state)
    register_changed_pos (&l->p, CHPOS_RELEASE_LOOSE_FLOOR);
}

ALLEGRO_BITMAP *
get_correct_falling_loose_floor_bitmap (ALLEGRO_BITMAP *b)
{
  if (b == dc_loose_floor_01
      || b == pc_loose_floor_01
      || b == de_loose_floor_01
      || b == pe_loose_floor_01
      || b == dv_loose_floor_01
      || b == pv_loose_floor_01) {
    switch (em) {
    case DUNGEON:
      switch (vm) {
      case CGA: return dc_loose_floor_01;
      case EGA: return de_loose_floor_01;
      case VGA: return dv_loose_floor_01;
      }
      break;
    case PALACE:
      switch (vm) {
      case CGA: return pc_loose_floor_01;
      case EGA: return pe_loose_floor_01;
      case VGA: return pv_loose_floor_01;
      }
      break;
    }
  } else if (b == dc_broken_floor
             || b == pc_broken_floor
             || b == de_broken_floor
             || b == pe_broken_floor
             || b == dv_broken_floor
             || b == pv_broken_floor) {
    switch (em) {
    case DUNGEON:
      switch (vm) {
      case CGA: return dc_broken_floor;
      case EGA: return de_broken_floor;
      case VGA: return dv_broken_floor;
      }
      break;
    case PALACE:
      switch (vm) {
      case CGA: return pc_broken_floor;
      case EGA: return pe_broken_floor;
      case VGA: return pv_broken_floor;
      }
      break;
    }
  }

  return NULL;
}

void
compute_loose_floor_fall (struct loose_floor *l)
{
  int speed = 3 * ++l->i;
  if (speed > 29) speed = 29;

  struct frame nf;
  struct frame_offset fo;
  fo.b = l->f.b;
  fo.dx = 0;
  fo.dy = speed;
  next_frame (&l->f, &nf, &fo);
  struct coord mbo_f, mbo_nf;
  struct pos fpmbo_f, nfpmbo_f, fpmbo_nf, nfpmbo_nf;
  enum confg fcmbo_f;
  survey (_mbo, posf, &l->f, &mbo_f, &fpmbo_f, &nfpmbo_f);
  fcmbo_f = fg (&fpmbo_f);
  survey (_mbo, posf, &nf, &mbo_nf, &fpmbo_nf, &nfpmbo_nf);

  struct pos p;

  /* hit character */
  int i;
  for (i = 0; i < anima_nmemb; i++) {
    struct coord kmt, ambo_f, ambo_nf; struct pos kpmt;
    struct anim *a = &anima[i];
    if (is_anim_dead (&a->f)
        || is_anim_fall (&a->f)
        || a->immortal
        || a->loose_floor_immune)
      continue;
    survey (_mt, pos, &a->f, &kmt, &kpmt, NULL);
    coord2room (&mbo_f, kpmt.room, &ambo_f);
    coord2room (&mbo_nf, kpmt.room, &ambo_nf);
    if (peq (&nfpmbo_f, &kpmt)
        && ambo_f.y <= kmt.y
        && ambo_nf.y >= kmt.y
        && ! a->hit_by_loose_floor
        && ! is_kid_hang_or_climb (&a->f)
        && ! is_kid_fall (&a->f)) {
      a->splash = true;
      a->current_lives--;
      a->uncouch_slowly = true;
      /* ensure kid doesn't couch in thin air (might occur when hit
         while jumping, for example) */
      place_on_the_ground (&a->f, &a->f.c);
      alert_guards (&kpmt);
      if (a->id == current_kid_id) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
      }
      if (a->current_lives <= 0) {
        a->p = kpmt;
        anim_die_suddenly (a);
        a->death_reason = LOOSE_FLOOR_DEATH;
      } else if (a->type == KID) {
        a->hit_by_loose_floor = true;
        play_audio (&hit_wall_audio, NULL, a->id);
        a->next_action = kid_couch;
      }
    }
  }

  /* fall */
  if (is_strictly_traversable (&fpmbo_f)
      || (peq (&fpmbo_f, &fpmbo_nf)
          && (fpmbo_nf.floor + 1) * PLACE_HEIGHT - mbo_nf.y > 8)) {
    /* the floor hit a rigid structure */
    if (is_rigid_con (&fpmbo_nf)) prel (&fpmbo_nf, &p, -1, 0);
    /* the floor continue to fall */
    else {
      l->f = nf;
      if (is_strictly_traversable (&fpmbo_nf)) l->p = fpmbo_nf;
      must_sort = true;
      return;
    }
    /* the floor hit the ground */
  } else {
    struct loose_floor *m;
    p = fpmbo_f;
    switch (fcmbo_f) {
    case LOOSE_FLOOR: /* loose floor isn't ground */
      m = loose_floor_at_pos (&fpmbo_f);
      if (m) m->remove = true;
      l->f = nf;
      l->f.b = get_correct_falling_loose_floor_bitmap (dv_broken_floor);
      l->p = p;
      l->i = 0;
      register_con_undo (&undo, &p,
                         NO_FLOOR, MIGNORE, MIGNORE,
                         false, false, false, false,
                         CHPOS_CHAIN_RELEASE_LOOSE_FLOOR,
                         "LOOSE FLOOR CHAIN RELEASE");
      play_audio (&broken_floor_audio, &p, -1);
      alert_guards (&p);
      return;
    case OPENER_FLOOR: break_opener_floor (&p); break;
    case CLOSER_FLOOR: break_closer_floor (&p); break;
    case SPIKES_FLOOR: break_spikes_floor (&p); break;
    case LEVEL_DOOR: break_level_door (&p); break;
    default: break;
    }
  }

  /* reach here only if the floor hit a rigid structure or the
     ground */
  enum confg f = fg (&p);
  if (f != LEVEL_DOOR && f != OPENER_FLOOR && f != CLOSER_FLOOR) {
    register_con_undo (&undo, &p,
                       BROKEN_FLOOR, MIGNORE, NO_ITEM,
                       false, false, false, false,
                       CHPOS_BREAK_LOOSE_FLOOR,
                       "LOOSE FLOOR BREAKING");
  }
  shake_loose_floor_row (&p);
  l->remove = true;
  play_audio (&broken_floor_audio, &p, -1);
  alert_guards (&p);
}

void
shake_loose_floor_row (struct pos *p)
{
  struct pos ps;

  for (ps = *p; is_shockwave_medium (&ps)
         && abs (p->place - ps.place) < SHOCKWAVE_RADIUS; ps.place--)
    if (fg (&ps) == LOOSE_FLOOR) {
      struct loose_floor *l = loose_floor_at_pos (&ps);
      if (l->action == NO_LOOSE_FLOOR_ACTION) {
        l->action = SHAKE_LOOSE_FLOOR;
        l->i = 0;
      }
    }

  for (prel (p, &ps, +0, +1); is_shockwave_medium (&ps)
         && abs (p->place - ps.place) < SHOCKWAVE_RADIUS; ps.place++)
    if (fg (&ps) == LOOSE_FLOOR) {
      struct loose_floor *l = loose_floor_at_pos (&ps);
      if (l->action == NO_LOOSE_FLOOR_ACTION) {
        l->action = SHAKE_LOOSE_FLOOR;
        l->i = 0;
      }
    }
}

void
sample_random_loose_floor (struct pos *p)
{
  switch (prandom (2)) {
  case 0: play_audio (&loose_floor_00_audio, p, -1);
  case 1: play_audio (&loose_floor_01_audio, p, -1);
  case 2: play_audio (&loose_floor_02_audio, p, -1);
  }
}

void
draw_falling_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  if (l->action == FALL_LOOSE_FLOOR) {
    struct frame f = l->f;
    frame2room (&f, room_view, &f.c);
    struct coord tr, br;
    struct pos fptr, nfptr, fpbr, nfpbr;
    survey (_tr, posf, &f, &tr, &fptr, &nfptr);
    survey (_br, posf, &f, &br, &fpbr, &nfpbr);
    f.b = get_correct_falling_loose_floor_bitmap (f.b);
    if (vm == VGA) f.b = apply_hue_palette (f.b);
    if (hgc) f.b = apply_palette (f.b, hgc_palette);
    draw_frame (bitmap, &f);

    int w = al_get_bitmap_width (f.b);
    int h = al_get_bitmap_height (f.b);

    set_target_bitmap (bitmap);
    al_set_clipping_rectangle (f.c.x, f.c.y, w, h);

    draw_confg_base (bitmap, &fptr, em, vm);
    draw_confg_left (bitmap, &fptr, em, vm, true);
    draw_confg_base (bitmap, &fpbr, em, vm);
    draw_confg_left (bitmap, &fpbr, em, vm, true);

    al_reset_clipping_rectangle ();
  } else return;
}

void
draw_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  if (l->action == FALL_LOOSE_FLOOR) return;
  else {
    switch (l->state) {
    case 0: draw_floor (bitmap, p, em, vm); break;
    case 1: draw_loose_floor_00 (bitmap, p, em, vm); break;
    case 2: draw_loose_floor_01 (bitmap, p, em, vm); break;
    }
  }
}

void
draw_loose_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  switch (l->state) {
  case 0: draw_floor_base (bitmap, p, em, vm); break;
  case 1: draw_loose_floor_00_base (bitmap, p, em, vm); break;
  case 2: draw_loose_floor_01_base (bitmap, p, em, vm); break;
  }
}

void
draw_loose_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  switch (l->state) {
  case 0: draw_floor_left (bitmap, p, em, vm); break;
  case 1: draw_loose_floor_00_left (bitmap, p, em, vm); break;
  case 2: draw_loose_floor_01_left (bitmap, p, em, vm); break;
  }
}

void
draw_loose_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  switch (l->state) {
  case 0: draw_floor_right (bitmap, p, em, vm); break;
  case 1: draw_loose_floor_00_right (bitmap, p, em, vm); break;
  case 2: draw_loose_floor_01_right (bitmap, p, em, vm); break;
  }
}

void
draw_loose_floor_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  draw_loose_floor_00_base (bitmap, p, em, vm);
  draw_loose_floor_00_left (bitmap, p, em, vm);
  draw_loose_floor_00_right (bitmap, p, em, vm);
}

void
draw_loose_floor_00_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *loose_floor_base_00 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: loose_floor_base_00 = dc_loose_floor_base_00; break;
    case EGA: loose_floor_base_00 = de_loose_floor_base_00; break;
    case VGA: loose_floor_base_00 = dv_loose_floor_base_00; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: loose_floor_base_00 = pc_loose_floor_base_00; break;
    case EGA: loose_floor_base_00 = pe_loose_floor_base_00; break;
    case VGA: loose_floor_base_00 = pv_loose_floor_base_00; break;
    }
    break;
  }

  if (vm == VGA) loose_floor_base_00 = apply_hue_palette (loose_floor_base_00);
  if (hgc) loose_floor_base_00 = apply_palette (loose_floor_base_00, hgc_palette);
  if (peq (p, &mouse_pos))
    loose_floor_base_00 = apply_palette (loose_floor_base_00, selection_palette);

  struct coord c;
  draw_bitmapc (loose_floor_base_00, bitmap, floor_base_coord (p, &c), 0);
}

void
draw_loose_floor_00_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *loose_floor_left_00 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: loose_floor_left_00 = dc_loose_floor_left_00; break;
    case EGA: loose_floor_left_00 = de_loose_floor_left_00; break;
    case VGA: loose_floor_left_00 = dv_loose_floor_left_00; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: loose_floor_left_00 = pc_loose_floor_left_00; break;
    case EGA: loose_floor_left_00 = pe_loose_floor_left_00; break;
    case VGA: loose_floor_left_00 = pv_loose_floor_left_00; break;
    }
    break;
  }

  if (vm == VGA) loose_floor_left_00 = apply_hue_palette (loose_floor_left_00);
  if (hgc) loose_floor_left_00 = apply_palette (loose_floor_left_00, hgc_palette);
  if (peq (p, &mouse_pos))
    loose_floor_left_00 = apply_palette (loose_floor_left_00, selection_palette);

  struct coord c;
  draw_bitmapc (loose_floor_left_00, bitmap, loose_floor_left_coord (p, &c), 0);
}

void
draw_loose_floor_00_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *loose_floor_right_00 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: loose_floor_right_00 = dc_loose_floor_right_00; break;
    case EGA: loose_floor_right_00 = de_loose_floor_right_00; break;
    case VGA: loose_floor_right_00 = dv_loose_floor_right_00; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: loose_floor_right_00 = pc_loose_floor_right_00; break;
    case EGA: loose_floor_right_00 = pe_loose_floor_right_00; break;
    case VGA: loose_floor_right_00 = pv_loose_floor_right_00; break;
    }
    break;
  }

  if (vm == VGA) loose_floor_right_00 = apply_hue_palette (loose_floor_right_00);
  if (hgc) loose_floor_right_00 = apply_palette (loose_floor_right_00, hgc_palette);
  if (peq (p, &mouse_pos))
    loose_floor_right_00 = apply_palette (loose_floor_right_00, selection_palette);

  struct coord c;
  draw_bitmapc (loose_floor_right_00, bitmap, loose_floor_right_coord (p, &c), 0);
}

void
draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  draw_loose_floor_01_base (bitmap, p, em, vm);
  draw_loose_floor_01_left (bitmap, p, em, vm);
  draw_loose_floor_01_right (bitmap, p, em, vm);
}

void
draw_loose_floor_01_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *loose_floor_base_01 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: loose_floor_base_01 = dc_loose_floor_base_01; break;
    case EGA: loose_floor_base_01 = de_loose_floor_base_01; break;
    case VGA: loose_floor_base_01 = dv_loose_floor_base_01; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: loose_floor_base_01 = pc_loose_floor_base_01; break;
    case EGA: loose_floor_base_01 = pe_loose_floor_base_01; break;
    case VGA: loose_floor_base_01 = pv_loose_floor_base_01; break;
    }
    break;
  }

  if (vm == VGA) loose_floor_base_01 = apply_hue_palette (loose_floor_base_01);
  if (hgc) loose_floor_base_01 = apply_palette (loose_floor_base_01, hgc_palette);
  if (peq (p, &mouse_pos))
    loose_floor_base_01 = apply_palette (loose_floor_base_01, selection_palette);

  struct coord c;
  draw_bitmapc (loose_floor_base_01, bitmap, floor_base_coord (p, &c), 0);
}

void
draw_loose_floor_01_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *loose_floor_left_01 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: loose_floor_left_01 = dc_loose_floor_left_01; break;
    case EGA: loose_floor_left_01 = de_loose_floor_left_01; break;
    case VGA: loose_floor_left_01 = dv_loose_floor_left_01; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: loose_floor_left_01 = pc_loose_floor_left_01; break;
    case EGA: loose_floor_left_01 = pe_loose_floor_left_01; break;
    case VGA: loose_floor_left_01 = pv_loose_floor_left_01; break;
    }
    break;
  }

  if (vm == VGA) loose_floor_left_01 = apply_hue_palette (loose_floor_left_01);
  if (hgc) loose_floor_left_01 = apply_palette (loose_floor_left_01, hgc_palette);
  if (peq (p, &mouse_pos))
    loose_floor_left_01 = apply_palette (loose_floor_left_01, selection_palette);

  struct coord c;
  draw_bitmapc (loose_floor_left_01, bitmap, floor_left_coord (p, &c), 0);
}

void
draw_loose_floor_01_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *loose_floor_right_01 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: loose_floor_right_01 = dc_loose_floor_right_01; break;
    case EGA: loose_floor_right_01 = de_loose_floor_right_01; break;
    case VGA: loose_floor_right_01 = dv_loose_floor_right_01; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: loose_floor_right_01 = pc_loose_floor_right_01; break;
    case EGA: loose_floor_right_01 = pe_loose_floor_right_01; break;
    case VGA: loose_floor_right_01 = pv_loose_floor_right_01; break;
    }
    break;
  }

  if (vm == VGA) loose_floor_right_01 = apply_hue_palette (loose_floor_right_01);
  if (hgc) loose_floor_right_01 = apply_palette (loose_floor_right_01, hgc_palette);
  if (peq (p, &mouse_pos))
    loose_floor_right_01 = apply_palette (loose_floor_right_01, selection_palette);

  struct coord c;
  draw_bitmapc (loose_floor_right_01, bitmap, loose_floor_right_coord (p, &c), 0);
}

struct coord *
loose_floor_left_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor + 50 - 1);
}

struct coord *
loose_floor_right_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1),
               PLACE_HEIGHT * p->floor + 50 - 1);
}

void
loose_floor_fall_debug (void)
{
  int i;
  for (i = 0; i < loose_floor_nmemb; i++) {
    struct loose_floor *l = &loose_floor[i];
    if (l->action != FALL_LOOSE_FLOOR) continue;
    struct pos pv; pos2room (&l->p, room_view, &pv);
    struct coord cv; coord2room (&l->f.c, room_view, &cv);
    printf ("(%i,%i,%i) == (%i,%i,%i) <%i,%i,%i> <%i,%i,%i> ? %i ? %i\n",
            l->p.room, l->p.floor, l->p.place,
            pv.room, pv.floor, pv.place,
            l->f.c.room, l->f.c.x, l->f.c.y,
            cv.room, cv.x, cv.y,
            peq (&l->p, &pv),
            cpos (&l->p, &pv));
    draw_falling_loose_floor (mr.cell[mr.dx][mr.dy].screen, &loose_floor[i].p, em, vm);
  }
}
