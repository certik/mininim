/*
  editor-dialog.h -- editor dialog module;

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

#ifndef MININIM_EDITOR_DIALOG_H
#define MININIM_EDITOR_DIALOG_H

/* functions */
int editor_key_cb (Ihandle *ih, int c);

void show_editor_gui (void);
void hide_editor_gui (void);
void update_editor_gui (void);

#endif	/* MININIM_EDITOR_DIALOG_H */
