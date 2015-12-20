/*
  random.h -- random module;

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

#ifndef FREEPOP_RANDOM_H
#define FREEPOP_RANDOM_H

#include <stdint.h>
#include "engine/pos.h"

/* random number generator seed */
extern uint32_t random_seed;

/* functions */
int prandom(int max);
int prandom_uniq (uint32_t seed, int length, int max);
int prandom_pos_uniq (struct pos *p, int i, int length, int max);
int prandom_pos (struct pos *p, int max);
void seedp (struct pos *p);
void unseedp (void);
int prandom_seq (uint32_t seed, int n, int p, int max);

#endif	/* FREEPOP_RANDOM_H */
