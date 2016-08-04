/*
  legacy-level.c -- legacy level module;

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

struct level legacy_level;
static int level_3_checkpoint;
static int shadow_id;
static int skeleton_id;
static bool played_sample;
static uint64_t mouse_timer;
static int mouse_id;
static bool coming_from_12;
static bool shadow_merged;
static bool met_jaffar;
static bool played_vizier_death_sample;

static int life_table[] = {4, 3, 3, 3, 3, 4, 5, 4, 4, 5, 5, 5, 4, 6, 0, 0};

static struct con room_0[FLOORS][PLACES] =
  {{{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
    {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
   {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
    {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
   {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
    {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}};

struct legacy_level lv;

static enum ltile get_tile (struct pos *p);
static enum lgroup get_group (enum ltile t);

static void load_legacy_level (int number);

void
play_legacy_level (int number)
{
  next_legacy_level (number);
  play_level (&legacy_level);
}

void
legacy_level_start (void)
{
  /* initialize some state */
  played_sample = false;
  shadow_id = mouse_id = skeleton_id = -1;
  stop_all_samples ();
  mouse_timer = 0;
  shadow_merged = false;
  met_jaffar = false;
  played_vizier_death_sample = false;

  if (coming_from_12) auto_rem_time_1st_cycle = -1;
  else auto_rem_time_1st_cycle = 24;

  /* get kid */
  struct anim *k = get_anim_by_id (0);

  if (coming_from_12) k->current_lives = current_lives;
  else k->current_lives = total_lives;

  /* make the kid turn as appropriate */
  switch (global_level.number) {
  case 1:
    k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
    k->i = -1; k->action = kid_turn;
    break;
  case 13:
    k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
    k->i = -1; k->action = kid_run;
    frame2room (&k->f, roomd (k->f.c.l, 23, RIGHT), &k->f.c);
    break;
  default: k->i = -1; k->action = kid_turn; break;
  }

  /* define camera's starting room */
  if (global_level.number == 7) {
    mr_center_room (1);
    camera_follow_kid = -1;
  } else if (global_level.number == 13) {
    mr_center_room (23);
    camera_follow_kid = -1;
  } else {
    mr_center_room (k->f.c.room);
    camera_follow_kid = k->id;
  }

  /* if in level 14 stop the timer */
  if (global_level.number == 14) play_time_stopped = true;

  /* in the first level */
  if (global_level.number == 1) {
    /* activate tile, usually to close the opened door in the starting
       room */
    struct pos p; new_pos (&p, &global_level, 5, 0, 2);
    activate_con (&p);
    /* if it's the first try make kid wait before uncouching */
    if (retry_level != 1) k->uncouch_slowly = true;
  }

  /* in the third level */
  if (global_level.number == 3) {
    /* if it's the first time playing the checkpoint hasn't been
       reached yet */
    if (retry_level != 3) level_3_checkpoint = false;
    /* if the checkpoint has been reached, respawn there */
    struct pos p; new_pos (&p, &global_level, 2, 0, 6);
    if (level_3_checkpoint) {
      struct pos plf; new_pos (&plf, &global_level, 7, 0, 4);
      register_con_undo (&undo, &plf,
                         NO_FLOOR, MIGNORE, MIGNORE,
                         true, true, false, true,
                         CHPOS_LOOSE_FLOOR_FALL,
                         "NO FLOOR");
      k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
      place_frame (&k->f, &k->f, kid_normal_00, &p,
                   k->f.dir == LEFT ? +22 : +31, +15);
      mr_center_room (2);
    }
  }

  /* in the tenth level unflip the screen vertically, (helpful if the
     kid has not drank the potion that would do it on its own) */
  if (global_level.number == 10) screen_flags &= ~ ALLEGRO_FLIP_VERTICAL;

  /* level 13 adjustements */
  coming_from_12 = false;
  if (global_level.number == 13 && retry_level == 13)
    global_level.nominal_number = 12;

  if (global_level.number == 13) {
    struct anim *v = get_anim_by_id (1);
    v->fight = false;
  }
}

void
legacy_level_special_events (void)
{
  struct pos p, pm, pms;
  struct coord m, ms;
  struct anim *k = get_anim_by_id (current_kid_id);

  /* in the first animation cycle */
  if (anim_cycle == 0 || anim_cycle == 1) {
    /* close level door the kid came from */
    if (con (&global_level.start_pos)->fg == LEVEL_DOOR) {
      struct level_door *ld = level_door_at_pos (&global_level.start_pos);
      if (anim_cycle == 0) ld->i = 0;
      if (anim_cycle == 1) ld->action = CLOSE_LEVEL_DOOR;
    }
  }

  /* in the first level, first try, play the suspense sound */
  if (global_level.number == 1 && anim_cycle == 12 && retry_level != 1)
    play_sample (suspense_sample, NULL, -1);

  /* in the third level */
  if (global_level.number == 3) {

    /* level 3 checkpoint */
    new_pos (&p, &global_level, 2, 0, 8);
    survey (_m, pos, &k->f, NULL, &pm, NULL);
    if (peq (&pm, &p)) {
      level_3_checkpoint = true;
      total_lives = k->total_lives;
      current_lives = k->current_lives;
      skill = k->skill;
    }

    struct anim *s = NULL;

    /* raise the skeleton as soon as the exit door is open and the
       kid reaches the second place of the room 1 */
    struct pos skeleton_floor_pos;
    new_pos (&skeleton_floor_pos, &global_level, 1, 1, 5);
    survey (_m, pos, &k->f, NULL, &pm, NULL);
    if (pm.room == 1
        && (pm.place == 2 || pm.place == 3)
        && con (&skeleton_floor_pos)->fg == SKELETON_FLOOR
        && get_exit_level_door (&global_level, 0)) {
      register_con_undo (&undo, &skeleton_floor_pos,
                         FLOOR, MIGNORE, MIGNORE,
                         true, true, false, true,
                         -1, "FLOOR");
      skeleton_id = create_anim (NULL, SKELETON, &skeleton_floor_pos, LEFT);
      s = &anima[skeleton_id];
      get_legacy_skill (2, &s->skill);
      s->has_sword = true;
      s->total_lives = INT_MAX;
      s->current_lives = INT_MAX;
      s->dont_draw_lives = true;
      s->p = skeleton_floor_pos;
      s->refraction = 16;
      raise_skeleton (s);
    }

    /* when the skeleton falls into room 3, place him at the center of
       the screen waiting for the kid */
    s = get_anim_by_id (skeleton_id);
    if (s) {
      survey (_m, pos, &s->f, NULL, &pm, NULL);
      new_pos (&p, &global_level, 3, 1, 4);
      if (s->f.c.room == 3 && pm.floor == 0
          && is_guard_fall (&s->f)) {
        s->f.dir = RIGHT;
        s->action = guard_normal;
        place_frame (&s->f, &s->f, skeleton_normal_00, &p, +16, +20);
      }
    }
  }

  /* in the fourth level */
  if (global_level.number == 4) {
    struct pos mirror_pos; new_pos (&mirror_pos, &global_level, 4, 0, 4);

    /* if the level door is open and the camera is on room 4, make
       the mirror appear */
    if (is_pos_visible (&mirror_pos)
        && con (&mirror_pos)->fg != MIRROR
        && get_exit_level_door (&global_level, 0)) {
      register_con_undo (&undo, &mirror_pos,
                         MIRROR, MIGNORE, MIGNORE,
                         true, true, true, true,
                         -1, "MIRROR");
      play_sample (suspense_sample, &mirror_pos, -1);
    }

    /* if the kid is crossing the mirror, make his shadow appear */
    struct mirror *m;
    if (con (&mirror_pos)->fg == MIRROR
        && (m = mirror_at_pos (&mirror_pos))
        && m->kid_crossing == k->id
        && shadow_id == -1) {
      k->current_lives = 1;
      int id = create_anim (k, 0, NULL, 0);
      struct anim *ks = &anima[id];
      ks->fight = false;
      ks->f.dir = (ks->f.dir == LEFT) ? RIGHT : LEFT;
      ks->f.flip ^= ALLEGRO_FLIP_HORIZONTAL;
      ks->controllable = false;
      ks->dont_draw_lives = true;
      ks->immortal = true;
      ks->shadow = true;
      shadow_id = id;
    } else m = NULL;

    /* make the kid's shadow run to the right until he disappears
       from view */
    if (shadow_id != -1) {
      struct anim *ks = get_anim_by_id (shadow_id);
      if (is_frame_visible_at_room (&ks->f, mirror_pos.room))
        ks->key.right = true;
      else {
        destroy_anim (ks);
        shadow_id = -1;
      }
    }
  }

  /* in the fifth level */
  if (global_level.number == 5) {
    struct pos door_pos; new_pos (&door_pos, &global_level, 24, 0, 1);
    struct pos potion_pos; new_pos (&potion_pos, &global_level, 24, 0, 3);
    struct pos shadow_pos; new_pos (&shadow_pos, &global_level, 24, 0, -1);

    /* if there is a door sufficiently open, and a potion in room 24,
       and the camera is there, create a kid's shadow to drink the
       potion */
    if (is_pos_visible (&potion_pos)
        && shadow_id == -1
        && con (&door_pos)->fg == DOOR
        && is_potion (&potion_pos)
        && door_at_pos (&door_pos)->i <= 25) {
      int id = create_anim (k, 0, NULL, 0);
      struct anim *ks = &anima[id];
      ks->shadow = true;
      ks->fight = false;
      ks->f.dir = RIGHT;
      ks->controllable = false;
      ks->action = kid_run;
      ks->i = -1;
      ks->immortal = true;
      ks->dont_draw_lives = true;
      place_frame (&ks->f, &ks->f, kid_normal_00, &shadow_pos,
                   +0, +15);
      shadow_id = id;
    }

    /* if the kid's shadow has been created, make him run until he
       reaches the potion, and then drink it.  Make him turn back
       running until he gets out of view */
    if (shadow_id != -1) {
      struct anim *ks = get_anim_by_id (shadow_id);
      if (is_potion (&potion_pos)) {
        survey (_m, pos, &ks->f, NULL, &pm, NULL);
        pos2room (&pm, 24, &pm);
        if (pm.place < potion_pos.place - 1) ks->key.right = true;
        else ks->key.shift = true;
      } else if (is_frame_visible_at_room (&ks->f, potion_pos.room))
        ks->key.left = true;
      else {
        destroy_anim (ks);
        shadow_id = -1;
      }
    }
  }

  /* in the sixth level */
  if (global_level.number == 6) {
    struct anim *ks;

    /* create kid's shadow to wait for kid at room 1 */
    if (shadow_id == -1) {
      struct pos shadow_pos; new_pos (&shadow_pos, &global_level, 1, 1, 1);
      int id = create_anim (k, 0, NULL, 0);
      ks = &anima[id];
      ks->fight = false;
      ks->shadow = true;
      ks->f.dir = RIGHT;
      ks->controllable = false;
      ks->action = kid_normal;
      ks->dont_draw_lives = true;
      ks->immortal = true;
      place_frame (&ks->f, &ks->f, kid_normal_00, &shadow_pos,
                   +9, +15);
      shadow_id = id;
    } else ks = get_anim_by_id (shadow_id);

    /* when kid enters room 1, play the suspense sound */
    if (k->f.c.room == 1
        && ! played_sample) {
      play_sample (suspense_sample, NULL, k->id);
      played_sample = true;
    }

    /* if kid opens the door and jumps to reach the other side of room
       1, make his shadow step over the closer floor */
    struct pos door_pos; new_pos (&door_pos, &global_level, 1, 1, 2);
    if (k->f.c.room == 1
        && k->action == kid_run_jump
        && k->i == 7
        && con (&door_pos)->fg == DOOR
        && door_at_pos (&door_pos)->i < DOOR_MAX_STEP) {
      ks->key.right = true;
      ks->key.shift = true;
    }

    /* when kid falls from room 1 to the room below it, quit to the
       next level */
    if (k->f.c.room == roomd (&global_level, 1, BELOW)
        && k->f.c.prev_room == 1
        && k->f.c.xd == BELOW) {
      total_lives = k->total_lives;
      current_lives = k->current_lives;
      skill = k->skill;
      quit_anim = NEXT_LEVEL;
    }
  }

  /* in the eighth level */
  if (global_level.number == 8) {
    struct pos mouse_pos; new_pos (&mouse_pos, &global_level, 16, 0, 12);
    struct anim *m = NULL;

    if (mouse_id != -1) m = get_anim_by_id (mouse_id);

    /* if the exit level door is open and the kid is at room 16,
       start counting (or continue if started already) for the mouse
       arrival */
    if (k->f.c.room == 16 && mouse_timer <= 138
        && get_exit_level_door (&global_level, 0)) mouse_timer++;

    /* if enough cycles have passed since the start of the countdown
       and the camera is at room 16, make the mouse appear */
    if (mouse_timer == 138 && is_room_visible (mouse_pos.room)) {
      mouse_id = create_anim (NULL, MOUSE, &mouse_pos, RIGHT);
      m = &anima[mouse_id];
      m->f.flip = ALLEGRO_FLIP_HORIZONTAL;
    }

    /* make the mouse disapear as soon as it goes out of view */
    if (m && m->action == mouse_run && m->f.dir == RIGHT
        && ! is_frame_visible_at_room (&m->f, mouse_pos.room)) {
        destroy_anim (m);
        mouse_id = -1;
    }
  }

  /* in the twelfth level */
  if (global_level.number == 12) {
    struct pos sword_pos; new_pos (&sword_pos, &global_level, 15, 0, 1);
    struct pos first_hidden_floor_pos;
    new_pos (&first_hidden_floor_pos, &global_level, 2, 0, 7);
    struct anim *ks = NULL;

    /* make the sword in room 15 disappear (kid's shadow has it) when
       the kid leaves room 18 to the right */
    if (k->f.c.room == roomd (&global_level, 18, RIGHT)
        && con (&sword_pos)->ext.item == SWORD)
      con (&sword_pos)->ext.item = NO_ITEM;

    /* make shadow fall in kid's front */
    survey (_m, pos, &k->f, NULL, &pm, NULL);
    if (shadow_id == -1
        && con (&sword_pos)->ext.item != SWORD
        && pm.room == 15 && pm.floor == 0 && pm.place < 6
        && ! shadow_merged) {
      struct pos shadow_pos;
      new_pos (&shadow_pos, &global_level, roomd (&global_level, 15, ABOVE), 2, 1);
      shadow_id = create_anim (NULL, SHADOW, &shadow_pos, RIGHT);
      ks = &anima[shadow_id];
      ks->fight = true;
      ks->controllable = false;
      ks->refraction = 12;
      ks->current_lives = k->current_lives;
      ks->total_lives = k->total_lives;
      ks->action = guard_normal;
      ks->f.dir = RIGHT;
      ks->glory_sample = true;
      get_legacy_skill (3, &ks->skill);
      guard_fall (ks);
      struct frameset *frameset = get_guard_fall_frameset (ks->type);
      place_frame (&ks->f, &ks->f, frameset[0].frame, &shadow_pos,
                   +9, +15);
    } else if (shadow_id != -1) ks = get_anim_by_id (shadow_id);

    /* if shadow has appeared but not merged yet */
    struct audio_sample *as;
    if (ks) {
      k->death_reason = ks->death_reason = SHADOW_FIGHT_DEATH;
      survey (_m, pos, &ks->f, &ms, &pms, NULL);
      survey (_m, pos, &k->f, &m, &pm, NULL);
      ks->p = pms;
      k->p = pm;

      ks->keep_sword_fast = k->keep_sword_fast = false;

      /* any harm caused to the shadow reflects to the kid */
      if (ks->action == guard_hit && ks->i == 0
          && is_attacking (k)) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
        play_sample (harm_sample, NULL, k->id);
        k->splash = true;
        k->current_lives--;
        kid_sword_hit (k);
      }

      /* any harm caused to the kid reflects to the shadow */
      if (k->action == kid_sword_hit && k->i == 0
          && is_attacking (ks)) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
        play_sample (guard_hit_sample, NULL, ks->id);
        ks->splash = true;
        ks->current_lives--;
        guard_hit (ks);
      }

      /* if the shadow dies, the kid dies */
      if (ks->current_lives <= 0 && k->current_lives > 0) {
        k->splash = true;
        kid_die (k);
      }

      /* if the kid dies, the shadow dies */
      if (k->current_lives <= 0 && ks->current_lives > 0) {
        ks->splash = true;
        guard_die (ks);
      }

      /* make the shadow disappear when the kid is dead */
      struct audio_sample *as;
      if (k->current_lives <= 0
          && (as = get_sample (success_suspense_sample, NULL, k->id))
          && get_sample_position (as->instance) >= 3.3
          && ! ks->invisible) {
        mr.flicker = 8;
        mr.color = WHITE;
        ks->invisible = true;
      }

      /* if the kid keep his sword, the shadow does the same */
      if (k->action == kid_keep_sword && ks->type == SHADOW) {
        ks->type = KID;
        ks->controllable = true;
        kid_keep_sword (ks);
        place_on_the_ground (&ks->f, &ks->f.c);
        ks->f.c.x += (ks->f.dir == LEFT) ? +8 : -8;
      }

      /* if the kid change his mind and take the sword again, the
         shadow becomes offensive again too */
      if (k->action == kid_take_sword && ks->type == KID) {
        ks->type = SHADOW;
        ks->controllable = false;
        guard_normal (ks);
        place_on_the_ground (&ks->f, &ks->f.c);
      }

      /* if both, the kid and the shadow, are not in fight mode and
         get close enough, a merge happens */
      if (ks->type == KID && pms.room == pm.room && pms.floor == pm.floor
          && abs (ms.x - m.x) <= 8) {
        destroy_anim (ks);
        shadow_id = -1; ks = NULL;
        shadow_merged = true;
        k = get_anim_by_id (current_kid_id);
        play_sample (success_sample, NULL, k->id);
        k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
        place_on_the_ground (&k->f, &k->f.c);
        kid_turn_run (k);
        k->current_lives = ++k->total_lives;
        mr.flicker = 8;
        mr.color = WHITE;
      }
      /* while the merge doesn't happen and neither the shadow nor the
         kid are in fight mode, the shadow's movements mirror the
         kid's */
      else if (ks->type == KID) {
        get_gamepad_state (&ks->key);
        bool l = ks->key.left;
        bool r = ks->key.right;
        ks->key.left = r;
        ks->key.right = l;
      }
      /* if the shadow and the kid has merged, one overlaps each other
         making them glow intermittently */
    } else if (shadow_merged
               && (as = get_sample (success_sample, NULL, k->id))
               && is_playing_sample_instance (as->instance))
      k->shadow = (anim_cycle % 2);
    /* after the success music has finished to play, the kid goes
       normal again */
    else if (shadow_merged) k->shadow = false;

    /* transform the empty space in hidden floors after the shadow and
       the kid has merged */
    if (shadow_merged
        && k->f.c.room == 2
        && con (&first_hidden_floor_pos)->fg == NO_FLOOR)
      for (new_pos (&p, &global_level, 2, 0, -4); p.place < PLACES;
           prel (&p, &p, +0, +1))
        if (con (&p)->fg == NO_FLOOR) con (&p)->fg = HIDDEN_FLOOR;

    /* when the kid enters room 23, go to the next level */
    if (k->f.c.room == 23) {
      coming_from_12 = true;
      total_lives = k->total_lives;
      current_lives = k->current_lives;
      skill = k->skill;
      quit_anim = NEXT_LEVEL;
    }
  }

  /* in the thirteenth level */
  if (global_level.number == 13) {

    /* make the top loose floors fall spontaneously */
    if (k->f.c.room == 16 || k->f.c.room == 23) {
      struct pos p; new_pos (&p, &global_level, k->f.c.room, -1, 0);
      p.place = prandom (9);
      activate_con (&p);
    }

    struct anim *v = get_anim_by_id (1);
    if (v) {
      /* play a special tune when meeting the vizier for the first
         time */
      if (k->f.c.room == v->f.c.room
          && v->action == guard_normal
          && ! met_jaffar) {
        play_sample (meet_vizier_sample, NULL, k->id);
        met_jaffar = true;
      }

      /* put the vizier in vigilance */
      struct audio_sample *as;
      if ((as = get_sample (meet_vizier_sample, NULL, k->id))
          && get_sample_position (as->instance) >= 2.2)
        v->fight = true;

      /* when the vizier dies do some video effect, play a tune, stop
         the play timer and display the remaining time */
      if (v->current_lives <= 0
          && ! played_vizier_death_sample) {
        mr.flicker = 12;
        mr.color = WHITE;
        stop_sample (meet_vizier_sample, NULL, k->id);
        play_sample (vizier_death_sample, NULL, k->id);
        played_vizier_death_sample = true;
        play_time_stopped = true;
        display_remaining_time ();
      }

      /* after vizier's death activate certain tile in order to open
         the exit level door */
      struct pos p; new_pos (&p, &global_level, 24, 0, 0);
      if (v->current_lives <= 0
          && k->f.c.room != v->f.c.room)
        activate_con (&p);
    }
  }

  /* in the fourteenth level */
  if (global_level.number == 14) {
    /* when the kid enters room 5, go to the next level */
    if (k->f.c.room == 5) {
      total_lives = k->total_lives;
      current_lives = k->current_lives;
      skill = k->skill;
      quit_anim = NEXT_LEVEL;
    }
  }
}

void
legacy_level_end (struct pos *p)
{
  struct anim *k = get_anim_by_id (current_kid_id);
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  /* end music samples to play per level */
  if (! played_sample) {
    stop_sample (floating_sample, NULL, k->id);
    switch (global_level.number) {
    case 1: case 2: case 3: case 5: case 6: case 7:
    case 8: case 9: case 10: case 11: case 12:
      si = play_sample (success_sample, NULL, k->id); break;
    case 4:
      si = play_sample (success_suspense_sample, NULL, k->id); break;
    case 13: break;
    }
    played_sample = true;
  }

  if (! is_playing_sample_instance (si)) quit_anim = NEXT_LEVEL;
}

void
next_legacy_level (int number)
{
  if (number < 1) number = 14;
  else if (number > 14) number = 1;
  load_legacy_level (number);
}

static void
load_legacy_level (int number)
{
  char *filename;
  xasprintf (&filename, "data/legacy-levels/%02d", number);

  ALLEGRO_FILE *lvf =
    load_resource (filename, (load_resource_f) xfopen_r);

  if (! lvf)
    error (-1, 0, "cannot read legacy level file %s", filename);

  al_fread (lvf, &lv, sizeof (lv));
  al_fclose (lvf);
  al_free (filename);

  interpret_legacy_level (number);
}

void
interpret_legacy_level (int number)
{
  struct pos p;
  new_pos (&p, &legacy_level, -1, -1, -1);

  memset (&legacy_level, 0, sizeof (legacy_level));
  legacy_level.number = number;
  if (number == 12 || number == 13) {
    if (coming_from_12) legacy_level.nominal_number = -1;
    else legacy_level.nominal_number = 12;
  } else if (number == 14) legacy_level.nominal_number = -1;
  else legacy_level.nominal_number = number;
  legacy_level.start = legacy_level_start;
  legacy_level.special_events = legacy_level_special_events;
  legacy_level.end = legacy_level_end;
  legacy_level.next_level = next_legacy_level;
  memcpy (&legacy_level.con[0], &room_0, sizeof (room_0));

  /* CUTSCENES: ok */
  switch (number) {
  default: break;
  case 1: legacy_level.cutscene = cutscene_01_05_11_anim; break;
  case 3: legacy_level.cutscene = cutscene_03_anim; break;
  case 5: legacy_level.cutscene = cutscene_01_05_11_anim; break;
  case 7: legacy_level.cutscene = cutscene_07_anim; break;
  case 8: legacy_level.cutscene = cutscene_08_anim; break;
  case 11: legacy_level.cutscene = cutscene_11_anim; break;
  case 14: legacy_level.cutscene = cutscene_14_anim; break;
  }

  /* SWORD: ok */
  legacy_level.has_sword = (number != 1);

  /* LINKS: ok */
  for (p.room = 1; p.room <= LROOMS; p.room++) {
    legacy_level.link[p.room].l = lv.link[p.room - 1][LD_LEFT];
    legacy_level.link[p.room].r = lv.link[p.room - 1][LD_RIGHT];
    legacy_level.link[p.room].a = lv.link[p.room - 1][LD_ABOVE];
    legacy_level.link[p.room].b = lv.link[p.room - 1][LD_BELOW];
  }

  /* FORETABLE and BACKTABLE: ok */
  for (p.room = 1; p.room <= LROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        struct con *c = &legacy_level.con[p.room][p.floor][p.place];

        uint8_t f = lv.foretable[p.room - 1][p.floor][p.place];
        uint8_t b = lv.backtable[p.room - 1][p.floor][p.place];

        bool mlf = f & 0x20 ? true : false;   /* loose floor modifier */
        enum ltile t = get_tile (&p);
        enum lgroup g = get_group (t);

        switch (t) {
        case LT_EMPTY: c->fg = NO_FLOOR; break;
        case LT_FLOOR: c->fg = FLOOR; break;
        case LT_SPIKES: c->fg = SPIKES_FLOOR; break;
        case LT_PILLAR: c->fg = PILLAR; break;
        case LT_GATE: c->fg = DOOR; c->bg = NO_BRICKS; break;
        case LT_STUCK_BUTTON: c->fg = STUCK_FLOOR; break;
        case LT_DROP_BUTTON: c->fg = CLOSER_FLOOR; break;
        case LT_TAPESTRY: c->fg = CARPET; break;
        case LT_BOTTOM_BIG_PILLAR: c->fg = BIG_PILLAR_BOTTOM; break;
        case LT_TOP_BIG_PILLAR: c->fg = BIG_PILLAR_TOP; c->bg = NO_BRICKS; break;
        case LT_POTION: c->fg = FLOOR; break;
        case LT_LOOSE_BOARD: c->fg = LOOSE_FLOOR; c->ext.cant_fall = mlf; break;
        case LT_TAPESTRY_TOP: c->fg = TCARPET; c->bg = NO_BRICKS; break;
        case LT_MIRROR: c->fg = MIRROR; break;
        case LT_DEBRIS: c->fg = BROKEN_FLOOR; break;
        case LT_RAISE_BUTTON: c->fg = OPENER_FLOOR; break;
        case LT_EXIT_LEFT: c->fg = FLOOR; c->bg = NO_BRICKS; break;
        case LT_EXIT_RIGHT: c->fg = LEVEL_DOOR; c->bg = NO_BRICKS; break;
        case LT_CHOPPER: c->fg = CHOPPER; break;
        case LT_TORCH: c->fg = FLOOR; c->bg = TORCH; break;
        case LT_WALL: c->fg = WALL; break;
        case LT_SKELETON: c->fg = SKELETON_FLOOR; break;
        case LT_SWORD: c->fg = FLOOR; c->ext.item = SWORD; break;
        case LT_BALCONY_LEFT: c->fg = FLOOR; c->bg = NO_BRICKS; break;
        case LT_BALCONY_RIGHT: c->fg = FLOOR; c->bg = BALCONY; break;
        case LT_LATTICE_PILLAR: c->fg = ARCH_BOTTOM; break;
        case LT_LATTICE_SUPPORT: c->fg = ARCH_TOP_MID; c->bg = NO_BRICKS; break;
        case LT_SMALL_LATTICE: c->fg = ARCH_TOP_SMALL; c->bg = NO_BRICKS; break;
        case LT_LATTICE_LEFT: c->fg = ARCH_TOP_LEFT; c->bg = NO_BRICKS; break;
        case LT_LATTICE_RIGHT: c->fg = ARCH_TOP_RIGHT; c->bg = NO_BRICKS; break;
        case LT_TORCH_WITH_DEBRIS: c->fg = BROKEN_FLOOR; c->bg = TORCH; break;
        case LT_NULL: break;    /* needless */
        default:
          error (-1, 0, "%s: unknown tile group (%i) at position (%i, %i, %i, %i)",
                 __func__, t, number, p.room, p.floor, p.place);
        }

        /* printf ("(%i, %i, %i): TILE!!!\n", */
        /*         p.room, p.floor, p.place); */

        struct pos pl; prel (&p, &pl, +0, -1);

        switch (g) {
        case LG_NONE: break;    /* ok */
        case LG_FREE:           /* ok */
          switch (b) {
          case LM_FREE_NOTHING_DUNGEON_BLUE_LINE_PALACE: /* ok */
            c->bg = (t == LT_EMPTY) ? NO_BRICKS : NO_BG; break;
          case LM_FREE_SPOT1_DUNGEON_NO_BLUE_LINE_PALACE: /* ok */
            c->bg = (t == LT_EMPTY) ? BRICKS_02 : BRICKS_00; break;
          case LM_FREE_SPOT2_DUNGEON_BLUE_LINE2_PALACE: /* ok */
            c->bg = (t == LT_EMPTY) ? BRICKS_03 : BRICKS_01; break;
          case LM_FREE_WINDOW: c->bg = WINDOW; break;
          case LM_FREE_SPOT3_DUNGEON_BLUE_LINE_PALACE: /* ok */
            c->bg = (t == LT_EMPTY) ? NO_BRICKS : NO_BG; break;
          }
          break;
        case LG_SPIKE:          /* ok */
          switch (b) {
          case LM_SPIKE_NORMAL: c->ext.step = 0; break;
          case LM_SPIKE_BARELY_OUT_1: c->ext.step = 1; break;
          case LM_SPIKE_HALF_OUT_1: c->ext.step = 2; break;
          case LM_SPIKE_FULLY_OUT_1: c->ext.step = 3; break;
          case LM_SPIKE_FULLY_OUT_2: c->ext.step = 4; break;
          case LM_SPIKE_OUT_1: c->ext.step = 5; break;
          case LM_SPIKE_OUT_2: c->ext.step = 6; break;
          case LM_SPIKE_HALF_OUT_2: c->ext.step = 7; break;
          case LM_SPIKE_BARELY_OUT_2: c->ext.step = 8; break;
          case LM_SPIKE_DISABLED: c->ext.step = 9; break;
            /* needless */
          case LM_SPIKE_WEIRD_1: break;
          case LM_SPIKE_WEIRD_2: break;
          case LM_SPIKE_WEIRD_3: break;
          }
          break;
        case LG_GATE:           /* ok */
          switch (b) {
          case LM_GATE_CLOSED: default: c->ext.step = DOOR_MAX_STEP; break;
          case LM_GATE_OPEN: c->ext.step = 0; break;
          }
          break;
        case LG_TAPEST:         /* ok */
          switch (b) {
          case LM_TAPEST_WITH_LATTICE: c->ext.design = ARCH_CARPET_LEFT; break;
          case LM_TAPEST_ALTERNATIVE_DESIGN: c->ext.design = CARPET_00; break;
          case LM_TAPEST_NORMAL: c->ext.design = CARPET_01; break;
          case LM_TAPEST_BLACK: break; /* needless */
            /* needless */
          case LM_TAPEST_WEIRD_1: break;
          case LM_TAPEST_WEIRD_2: break;
          case LM_TAPEST_WEIRD_3: break;
          case LM_TAPEST_WEIRD_4: break;
          case LM_TAPEST_WEIRD_5: break;
          case LM_TAPEST_WEIRD_6: break;
          case LM_TAPEST_WEIRD_7: break;
          }
          break;
        case LG_POTION:         /* ok */
          switch (b) {
          case LM_POTION_EMPTY: c->ext.item = EMPTY_POTION; break;
          case LM_POTION_HEALTH_POINT: c->ext.item = SMALL_LIFE_POTION; break;
          case LM_POTION_LIFE: c->ext.item = BIG_LIFE_POTION; break;
          case LM_POTION_FEATHER_FALL: c->ext.item = FLOAT_POTION; break;
          case LM_POTION_INVERT: c->ext.item = FLIP_POTION; break;
          case LM_POTION_POISON: c->ext.item = SMALL_POISON_POTION; break;
          case LM_POTION_OPEN: c->ext.item = ACTIVATION_POTION; break;
          }
          break;
        case LG_TTOP:           /* ok */
          switch (b) {
          case LM_TTOP_WITH_LATTICE: c->ext.design = ARCH_CARPET_LEFT; break;
          case LM_TTOP_ALTERNATIVE_DESIGN:
            c->ext.design = get_tile (&pl) == LT_LATTICE_SUPPORT ?
              ARCH_CARPET_RIGHT_00 : CARPET_00; break;
          case LM_TTOP_NORMAL:
            c->ext.design = get_tile (&pl) == LT_LATTICE_SUPPORT ?
              ARCH_CARPET_RIGHT_01 : CARPET_01; break;
            /* needless */
          case LM_TTOP_BLACK_01: break;
          case LM_TTOP_BLACK_02: break;
          case LM_TTOP_WITH_ALTERNATIVE_DESIGN_AND_BOTTOM: break;
          case LM_TTOP_WITH_BOTTOM: break;
          case LM_TTOP_WITH_WINDOW: break;
          case LM_TTOP_WEIRD_1: break;
          case LM_TTOP_WEIRD_2: break;
          case LM_TTOP_WEIRD_3: break;
          case LM_TTOP_WEIRD_4: break;
          case LM_TTOP_WEIRD_5: break;
          case LM_TTOP_WEIRD_6: break;
          case LM_TTOP_WEIRD_7: break;
          }
          break;
        case LG_CHOMP:          /* ok */
          switch (b & 0x7F) {
          case LM_CHOMP_NORMAL: c->ext.step = 0; break;
          case LM_CHOMP_HALF_OPEN: c->ext.step = 1; break;
          case LM_CHOMP_CLOSED: c->ext.step = 2; break;
          case LM_CHOMP_PARTIALLY_OPEN: c->ext.step = 3; break;
          case LM_CHOMP_EXTRA_OPEN: c->ext.step = 4; break;
          case LM_STUCK_OPEN: c->ext.step = 5; break;
          }
          c->ext.step |= (b & 0x80); /* bloody status */
          break;
        case LG_WALL:           /* ok */
          switch (b) {
          case LM_WALL_MARK: c->bg = NO_BG; break;
          case LM_WALL_NO_MARK: c->bg = NO_BRICKS; break;
          }
          break;
        case LG_EXIT:           /* ok */
          c->ext.step = LEVEL_DOOR_MAX_STEP;
          switch (b) {
          case LM_EXIT_HALF_OPEN: c->ext.step = 20; break;
            /* needless */
          case LM_EXIT_MORE_OPEN_1:
          case LM_EXIT_MORE_OPEN_2:
          case LM_EXIT_MORE_OPEN_3:
          case LM_EXIT_MORE_OPEN_4:
          case LM_EXIT_MORE_OPEN_5:
          case LM_EXIT_MORE_OPEN_6: break;
          case LM_EXIT_EVEN_MORE_OPEN: break;
          case LM_EXIT_MOST_OPEN: break;
          }
          break;
        case LG_EVENT: c->ext.event = b; break; /* ok */
        default:
          error (-1, 0, "%s: unknown tile group (%i) at position (%i, %i, %i, %i)",
                 __func__, g, number, p.room, p.floor, p.place);
        }
      }

  /* EVENTS: ok */
  int i;
  for (i = 0; i < LEVENTS; i++) {
    struct level_event *e = &legacy_level.event[i];
    int l = lv.door_1[i] & 0x1F;
    new_pos (&e->p, &legacy_level,
             (lv.door_2[i] >> 3) | ((lv.door_1[i] & 0x60) >> 5),
             l / PLACES, l % PLACES);
    if (get_tile (&e->p) == LT_EXIT_LEFT) e->p.place++;
    e->next = lv.door_1[i] & 0x80 ? false : true;
  }

  /* START POSITION: ok */
  struct pos *sp = &legacy_level.start_pos;
  sp->room = lv.start_position[0];
  sp->floor = lv.start_position[1] / PLACES;
  sp->place = lv.start_position[1] % PLACES;

  /* START DIRECTION: ok */
  enum dir *sd = &legacy_level.start_dir;
  *sd = lv.start_position[2] ? LEFT : RIGHT;

  /* GUARD LOCATION, DIRECTION, SKILL and COLOR: ok */
  for (i = 0; i < LROOMS; i++) {
    struct guard *g = &legacy_level.guard[i + 1];

    if (lv.guard_location[i] > 29) {
      g->type = NO_ANIM;
      continue;
    }

    /* TYPE AND STYLE: ok */
    switch (number) {
    case 3: g->type = SKELETON; g->style = 0; break;
    case 6: g->type = FAT_GUARD; g->style = 1; break;
    case 12: g->type = SHADOW; g->style = 0; break;
    case 13: g->type = VIZIER; g->style = 0; break;
    default: g->type = GUARD;
      g->style = lv.guard_color[i]; break;
    }

    /* LOCATION: ok */
    new_pos (&g->p, &legacy_level, i + 1,
             lv.guard_location[i] / PLACES,
             lv.guard_location[i] % PLACES);

    /* DIRECTION: ok */
    g->dir = lv.guard_direction[i] ? LEFT : RIGHT;

    /* SKILL: ok */
    get_legacy_skill (lv.guard_skill[i], &g->skill);
    g->total_lives = life_table[number];

    /* printf ("(%i, %i, %i), style: %i\n", */
    /*         g->p.room, g->p.floor, g->p.place, g->style); */
  }

  /* define the enviroment mode based on the level */
  switch (legacy_level.number) {
  case 4: case 5: case 6: case 10: case 11: case 14:
    legacy_level.em = PALACE; break;
  default: legacy_level.em = DUNGEON; break;
  }

  /* define hue palettes based on the level */
  switch (number) {
  default: legacy_level.hue = HUE_NONE; break;
  case 3: case 7: legacy_level.hue = HUE_GREEN; break;
  case 8: case 9: legacy_level.hue = HUE_GRAY; break;
  case 12: case 13: legacy_level.hue = HUE_YELLOW; break;
  case 14: legacy_level.hue = HUE_BLUE; break;
  }
}

struct skill *
get_legacy_skill (int i, struct skill *skill)
{
  switch (i) {
  case 0: default:
    skill->attack_prob = 23;
    skill->counter_attack_prob = -1;
    skill->defense_prob = -1;
    skill->counter_defense_prob = -1;
    skill->advance_prob = 99;
    skill->return_prob = -1;
    skill->refraction = 16;
    skill->extra_life = 0;
    break;
  case 1:
    skill->attack_prob = 38;
    skill->counter_attack_prob = -1;
    skill->defense_prob = 58;
    skill->counter_defense_prob = 23;
    skill->advance_prob = 77;
    skill->return_prob = 21;
    skill->refraction = 16;
    skill->extra_life = 0;
    break;
  case 2:
    skill->attack_prob = 23;
    skill->counter_attack_prob = -1;
    skill->defense_prob = 58;
    skill->counter_defense_prob = 23;
    skill->advance_prob = 77;
    skill->return_prob = 21;
    skill->refraction = 16;
    skill->extra_life = 0;
    break;
  case 3:
    skill->attack_prob = 23;
    skill->counter_attack_prob = 1;
    skill->defense_prob = 77;
    skill->counter_defense_prob = 38;
    skill->advance_prob = 77;
    skill->return_prob = 21;
    skill->refraction = 16;
    skill->extra_life = 0;
    break;
  case 4:
    skill->attack_prob = 23;
    skill->counter_attack_prob = 1;
    skill->defense_prob = 77;
    skill->counter_defense_prob = 38;
    skill->advance_prob = 99;
    skill->return_prob = -1;
    skill->refraction = 8;
    skill->extra_life = 1;
    break;
  case 5:
    skill->attack_prob = 15;
    skill->counter_attack_prob = 68;
    skill->defense_prob = 99;
    skill->counter_defense_prob = 56;
    skill->advance_prob = 99;
    skill->return_prob = -1;
    skill->refraction = 8;
    skill->extra_life = 0;
    break;
  case 6:
    skill->attack_prob = 38;
    skill->counter_attack_prob = 5;
    skill->defense_prob = 77;
    skill->counter_defense_prob = 38;
    skill->advance_prob = 77;
    skill->return_prob = 21;
    skill->refraction = 8;
    skill->extra_life = 0;
    break;
  case 7:
    skill->attack_prob = 85;
    skill->counter_attack_prob = 2;
    skill->defense_prob = 97;
    skill->counter_defense_prob = 97;
    skill->advance_prob = -1;
    skill->return_prob = 99;
    skill->refraction = 8;
    skill->extra_life = 0;
    break;
  case 8:
    skill->attack_prob = -1;
    skill->counter_attack_prob = -1;
    skill->defense_prob = -1;
    skill->counter_defense_prob = -1;
    skill->advance_prob = -1;
    skill->return_prob = 99;
    skill->refraction = 0;
    skill->extra_life = 0;
    break;
  case 9:
    skill->attack_prob = 18;
    skill->counter_attack_prob = 99;
    skill->defense_prob = 99;
    skill->counter_defense_prob = 56;
    skill->advance_prob = 99;
    skill->return_prob = -1;
    skill->refraction = 8;
    skill->extra_life = 0;
    break;
  case 10:
    skill->attack_prob = 12;
    skill->counter_attack_prob = 99;
    skill->defense_prob = 99;
    skill->counter_defense_prob = 99;
    skill->advance_prob = 38;
    skill->return_prob = 60;
    skill->refraction = 0;
    skill->extra_life = 0;
    break;
  case 11:
    skill->attack_prob = 18;
    skill->counter_attack_prob = 58;
    skill->defense_prob = 99;
    skill->counter_defense_prob = 68;
    skill->advance_prob = 38;
    skill->return_prob = 60;
    skill->refraction = 0;
    skill->extra_life = 0;
  }

  return skill;
}

static enum ltile
get_tile (struct pos *p)
{
  if (! is_valid_pos (p)) return LT_NULL;
  struct pos np; npos (p, &np);
  return lv.foretable[np.room - 1][np.floor][np.place] & 0x1F;
}

static enum lgroup
get_group (enum ltile t)
{
  switch (t) {
  case LT_PILLAR: case LT_STUCK_BUTTON: case LT_BOTTOM_BIG_PILLAR:
  case LT_TOP_BIG_PILLAR: case LT_LOOSE_BOARD: case LT_MIRROR:
  case LT_DEBRIS: case LT_TORCH: case LT_SKELETON: case LT_SWORD:
  case LT_BALCONY_LEFT: case LT_BALCONY_RIGHT: case LT_LATTICE_PILLAR:
  case LT_LATTICE_SUPPORT: case LT_SMALL_LATTICE: case LT_LATTICE_LEFT:
  case LT_LATTICE_RIGHT: case LT_TORCH_WITH_DEBRIS: case LT_NULL:
    return LG_NONE;
  case LT_EMPTY: case LT_FLOOR: return LG_FREE;
  case LT_SPIKES: return LG_SPIKE;
  case LT_GATE: return LG_GATE;
  case LT_TAPESTRY: return LG_TAPEST;
  case LT_POTION: return LG_POTION;
  case LT_TAPESTRY_TOP: return LG_TTOP;
  case LT_CHOPPER: return LG_CHOMP;
  case LT_WALL: return LG_WALL;
  case LT_EXIT_LEFT: case LT_EXIT_RIGHT: return LG_EXIT;
  case LT_DROP_BUTTON: case LT_RAISE_BUTTON: return LG_EVENT;
  default:
    error (-1, 0, "%s: unknown tile (%i)", __func__, t);
  }
  return LG_NONE;
}
