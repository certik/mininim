/*
  mininim.h -- MININIM main module;

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

#ifndef MININIM_MININIM_H
#define MININIM_MININIM_H

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <error.h>
#include <math.h>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "constants.h"
#include "types.h"
#include "colors.h"
#include "array.h"
#include "audio.h"
#include "video.h"
#include "event.h"
#include "keyboard.h"
#include "memory.h"
#include "random.h"
#include "timer.h"
#include "video.h"
#include "xstdio.h"
#include "anim.h"
#include "arch.h"
#include "balcony.h"
#include "big-pillar.h"
#include "bricks.h"
#include "broken-floor.h"
#include "carpet.h"
#include "chopper.h"
#include "clock.h"
#include "closer-floor.h"
#include "consistency.h"
#include "cutscenes.h"
#include "door.h"
#include "fight.h"
#include "fire.h"
#include "floor.h"
#include "jaffar.h"
#include "level-door.h"
#include "level.h"
#include "loose-floor.h"
#include "mirror.h"
#include "mouse.h"
#include "opener-floor.h"
#include "physics.h"
#include "pillar.h"
#include "pos.h"
#include "potion.h"
#include "princess.h"
#include "room.h"
#include "samples.h"
#include "skeleton-floor.h"
#include "spikes-floor.h"
#include "stars.h"
#include "sword.h"
#include "torch.h"
#include "window.h"
#include "guard.h"
#include "guard-attack.h"
#include "guard-defense.h"
#include "guard-die.h"
#include "guard-fall.h"
#include "guard-hit.h"
#include "guard-normal.h"
#include "guard-vigilant.h"
#include "guard-walkb.h"
#include "guard-walkf.h"
#include "kid.h"
#include "kid-normal.h"
#include "kid-walk.h"
#include "kid-start-run.h"
#include "kid-run.h"
#include "kid-stop-run.h"
#include "kid-turn-run.h"
#include "kid-couch.h"
#include "kid-turn.h"
#include "kid-stabilize.h"
#include "kid-jump.h"
#include "kid-vjump.h"
#include "kid-run-jump.h"
#include "kid-misstep.h"
#include "kid-hang.h"
#include "kid-hang-wall.h"
#include "kid-hang-free.h"
#include "kid-climb.h"
#include "kid-unclimb.h"
#include "kid-fall.h"
#include "kid-drink.h"
#include "kid-raise-sword.h"
#include "kid-keep-sword.h"
#include "kid-take-sword.h"
#include "kid-sword-normal.h"
#include "kid-sword-walkf.h"
#include "kid-sword-walkb.h"
#include "kid-sword-defense.h"
#include "kid-sword-attack.h"
#include "kid-sword-hit.h"
#include "kid-die.h"
#include "kid-stairs.h"
#include "consistency-level.h"
#include "legacy-level.h"
#include "level-1.h"
#include "wall.h"
#include "wall-dcpc.h"
#include "wall-depedv.h"
#include "wall-pv.h"

/* functions */
int max_int (int a, int b);
int min_int (int a, int b);

/* variables */
extern ALLEGRO_TIMER *play_time;
extern enum em em;
extern enum vm vm;
extern char *program_name;

#endif	/* MININIM_MININIM_H */
