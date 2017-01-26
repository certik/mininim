/*
  ui.c -- user interface module;

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

#if MENU_FEATURE
static ALLEGRO_MENU *append_menu;
static int append_menu_index;

static void start_menu (ALLEGRO_MENU *parent, uint16_t id);
static bool menu_item (char const *title, uint16_t id, int flags,
                       ALLEGRO_BITMAP *icon, bool has_submenu);
static bool menu_sitem (char const *title, uint16_t id, bool enabled,
                        ALLEGRO_BITMAP *icon);
static bool menu_ditem (bool first, char const *title0, char const *title1,
                        uint16_t id0, uint16_t id1, bool enabled,
                        ALLEGRO_BITMAP *icon0, ALLEGRO_BITMAP *icon1);
static bool menu_sub (char const *title, uint16_t id, bool enabled,
                      ALLEGRO_BITMAP *icon);
static bool menu_citem (char const *title, uint16_t id, bool enabled,
                        bool checked, ALLEGRO_BITMAP *icon);
static void menu_sep (void);
static void end_menu (void);
#endif

#if MENU_FEATURE
ALLEGRO_MENU *main_menu;

static void menu_widget_pause (void);
static void menu_widget_speed (void);
#endif

ALLEGRO_BITMAP *small_logo_icon,
  *open_icon, *save_icon, *reload_icon, *quit_icon,
  *full_screen_icon, *windows_icon, *camera_icon, *play_icon,
  *record_icon, *stop_icon, *eject_icon, *backward_icon,
  *forward_icon, *pause_icon, *previous_icon, *next_icon,
  *screen_icon, *right_icon, *card_icon, *dungeon_icon,
  *palace_icon, *green_icon, *gray_icon, *yellow_icon,
  *blue_icon, *black_icon, *vga_icon, *ega_icon, *cga_icon,
  *hgc_icon, *vertical_icon, *horizontal_icon,
  *keyboard_icon, *joystick_icon, *cancel_icon, *clock_icon,
  *edit_icon, *joystick2_icon, *undo_icon, *redo_icon,
  *screensaver_icon, *joystick3_icon, *volume_off_icon,
  *volume_low_icon, *volume_medium_icon, *volume_high_icon;




ALLEGRO_BITMAP *
load_icon (char *filename)
{
  if (WINDOWS_PORT) return load_scaled_memory_bitmap (filename, 13, 13, 0);
  else return load_scaled_memory_bitmap (filename, 16, 16, 0);
}

ALLEGRO_BITMAP *
micon (ALLEGRO_BITMAP *bitmap)
{
  return clone_memory_bitmap (bitmap);
}

ALLEGRO_BITMAP *
micon_flags (ALLEGRO_BITMAP *bitmap, int flags)
{
  if (! bitmap) return NULL;
  return clone_scaled_memory_bitmap
    (bitmap, al_get_bitmap_width (bitmap),
     al_get_bitmap_height (bitmap), flags);
}

void
load_icons (void)
{
  small_logo_icon = load_icon (SMALL_LOGO_ICON);
  open_icon = load_icon (OPEN_ICON);
  save_icon = load_icon (SAVE_ICON);
  reload_icon = load_icon (RELOAD_ICON);
  quit_icon = load_icon (QUIT_ICON);
  full_screen_icon = load_icon (FULL_SCREEN_ICON);
  windows_icon = load_icon (WINDOWS_ICON);
  camera_icon = load_icon (CAMERA_ICON);
  play_icon = load_icon (PLAY_ICON);
  record_icon = load_icon (RECORD_ICON);
  stop_icon = load_icon (STOP_ICON);
  eject_icon = load_icon (EJECT_ICON);
  backward_icon = load_icon (BACKWARD_ICON);
  forward_icon = load_icon (FORWARD_ICON);
  pause_icon = load_icon (PAUSE_ICON);
  previous_icon = load_icon (PREVIOUS_ICON);
  next_icon = load_icon (NEXT_ICON);
  screen_icon = load_icon (SCREEN_ICON);
  right_icon = load_icon (RIGHT_ICON);
  card_icon = load_icon (CARD_ICON);
  dungeon_icon = load_icon (DUNGEON_ICON);
  palace_icon = load_icon (PALACE_ICON);
  green_icon = load_icon (GREEN_ICON);
  gray_icon = load_icon (GRAY_ICON);
  yellow_icon = load_icon (YELLOW_ICON);
  blue_icon = load_icon (BLUE_ICON);
  black_icon = load_icon (BLACK_ICON);
  vga_icon = load_icon (VGA_ICON);
  ega_icon = load_icon (EGA_ICON);
  cga_icon = load_icon (CGA_ICON);
  hgc_icon = load_icon (HGC_ICON);
  vertical_icon = load_icon (VERTICAL_ICON);
  horizontal_icon = load_icon (HORIZONTAL_ICON);
  keyboard_icon = load_icon (KEYBOARD_ICON);
  joystick_icon = load_icon (JOYSTICK_ICON);
  cancel_icon = load_icon (CANCEL_ICON);
  clock_icon = load_icon (CLOCK_ICON);
  edit_icon = load_icon (EDIT_ICON);
  joystick2_icon = load_icon (JOYSTICK2_ICON);
  undo_icon = load_icon (UNDO_ICON);
  redo_icon = load_icon (REDO_ICON);
  screensaver_icon = load_icon (SCREENSAVER_ICON);
  joystick3_icon = load_icon (JOYSTICK3_ICON);
  volume_off_icon = load_icon (VOLUME_OFF_ICON);
  volume_low_icon = load_icon (VOLUME_LOW_ICON);
  volume_medium_icon = load_icon (VOLUME_MEDIUM_ICON);
  volume_high_icon = load_icon (VOLUME_HIGH_ICON);
}

void
unload_icons (void)
{
  destroy_bitmap (small_logo_icon);
  destroy_bitmap (open_icon);
  destroy_bitmap (save_icon);
  destroy_bitmap (reload_icon);
  destroy_bitmap (quit_icon);
  destroy_bitmap (full_screen_icon);
  destroy_bitmap (windows_icon);
  destroy_bitmap (camera_icon);
  destroy_bitmap (play_icon);
  destroy_bitmap (record_icon);
  destroy_bitmap (stop_icon);
  destroy_bitmap (eject_icon);
  destroy_bitmap (backward_icon);
  destroy_bitmap (forward_icon);
  destroy_bitmap (pause_icon);
  destroy_bitmap (previous_icon);
  destroy_bitmap (next_icon);
  destroy_bitmap (screen_icon);
  destroy_bitmap (right_icon);
  destroy_bitmap (card_icon);
  destroy_bitmap (dungeon_icon);
  destroy_bitmap (palace_icon);
  destroy_bitmap (green_icon);
  destroy_bitmap (gray_icon);
  destroy_bitmap (yellow_icon);
  destroy_bitmap (blue_icon);
  destroy_bitmap (black_icon);
  destroy_bitmap (vga_icon);
  destroy_bitmap (ega_icon);
  destroy_bitmap (cga_icon);
  destroy_bitmap (hgc_icon);
  destroy_bitmap (vertical_icon);
  destroy_bitmap (horizontal_icon);
  destroy_bitmap (keyboard_icon);
  destroy_bitmap (joystick_icon);
  destroy_bitmap (cancel_icon);
  destroy_bitmap (clock_icon);
  destroy_bitmap (edit_icon);
  destroy_bitmap (joystick2_icon);
  destroy_bitmap (undo_icon);
  destroy_bitmap (redo_icon);
  destroy_bitmap (screensaver_icon);
  destroy_bitmap (joystick3_icon);
  destroy_bitmap (volume_off_icon);
  destroy_bitmap (volume_low_icon);
  destroy_bitmap (volume_medium_icon);
  destroy_bitmap (volume_high_icon);
}





#if MENU_FEATURE
void
start_menu (ALLEGRO_MENU *parent, uint16_t id)
{
  if (id > 0) append_menu = al_find_menu (parent, id);
  else append_menu = parent;
  assert (append_menu);
  append_menu_index = 0;
}
#endif

#if MENU_FEATURE
bool
menu_item (char const *title, uint16_t id, int flags,
           ALLEGRO_BITMAP *icon, bool has_submenu)
{
  ALLEGRO_MENU *submenu = al_find_menu (append_menu, id);;

  if (has_submenu && ! submenu) {
    submenu = al_create_menu ();
    goto replace;
  } else if (! has_submenu && submenu) {
    submenu = NULL;
    goto replace;
  }

  ALLEGRO_MENU *f_menu;
  int f_index;

  if (al_find_menu_item (append_menu, id, &f_menu, &f_index)) {
    if (f_menu != append_menu || f_index != append_menu_index)
      goto replace;

    int f_flags = al_get_menu_item_flags (append_menu, -append_menu_index);
    if ((f_flags & ~ALLEGRO_MENU_ITEM_DISABLED)
        != (flags & ~ALLEGRO_MENU_ITEM_DISABLED)) goto replace;

    al_set_menu_item_caption (append_menu, -append_menu_index, title);
    al_set_menu_item_flags (append_menu, -append_menu_index, flags);
    al_set_menu_item_icon (append_menu, -append_menu_index, icon);

    append_menu_index++;

    return true;
  }

 replace:
  al_remove_menu_item (append_menu, -append_menu_index);
  return al_insert_menu_item (append_menu, -(append_menu_index++), title,
                              id, flags, icon, submenu);
}
#endif

#if MENU_FEATURE
bool
menu_sitem (char const *title, uint16_t id, bool enabled,
            ALLEGRO_BITMAP *icon)
{
  return menu_item (title, id, enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                    icon, false);
}
#endif

#if MENU_FEATURE
bool
menu_ditem (bool first, char const *title0, char const *title1,
            uint16_t id0, uint16_t id1, bool enabled,
            ALLEGRO_BITMAP *icon0, ALLEGRO_BITMAP *icon1)
{
  return menu_item (first ? title0 : title1,
                    first ? id0 : id1,
                    enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                    first ? icon0 : icon1, false);
}
#endif

#if MENU_FEATURE
bool
menu_sub (char const *title, uint16_t id, bool enabled, ALLEGRO_BITMAP *icon)
{
  return menu_item (title, id, enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                    icon, true);
}
#endif

#if MENU_FEATURE
bool
menu_citem (char const *title, uint16_t id, bool enabled, bool checked,
            ALLEGRO_BITMAP *icon)
{
  int flags = enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED;
  flags |= checked ? ALLEGRO_MENU_ITEM_CHECKED : 0;
  bool r = menu_item (title, id, flags, checked ? NULL : icon, false);
  if (checked) destroy_bitmap (icon);
  return r;
}
#endif

#if MENU_FEATURE
void
menu_sep (void)
{
  if (WINDOWS_PORT)
    menu_item (NULL, NO_MID, 0, NULL, false);
  else
    menu_item ("", NO_MID, ALLEGRO_MENU_ITEM_DISABLED, NULL, false);
}
#endif

#if MENU_FEATURE
void
end_menu (void)
{
  while (al_remove_menu_item (append_menu, -append_menu_index));
}
#endif





void
show_menu (void)
{
#if MENU_FEATURE
  if (display_mode < 0) {
    al_set_display_menu (display, main_menu);
    show_mouse_cursor ();
  }
#endif
}

void
hide_menu (void)
{
#if MENU_FEATURE
  if (is_fullscreen ()) {
    al_remove_display_menu (display);
    hide_mouse_cursor ();
  }
#endif
}

void
toggle_menu_visibility (void)
{
  if (is_showing_menu ()) hide_menu ();
  else show_menu ();
}

bool
is_showing_menu (void)
{
#if MENU_FEATURE
  return al_get_display_menu (display) ? true : false;
#else
  return false;
#endif
}

void
enable_menu (bool enable)
{
#if MENU_FEATURE
  int i = 0;
  int flags;
  while ((flags = al_get_menu_item_flags (main_menu, i--)) != -1) {
    if (enable) flags &= ~ALLEGRO_MENU_ITEM_DISABLED;
    else flags |= ALLEGRO_MENU_ITEM_DISABLED;
    al_set_menu_item_flags (main_menu, i + 1, flags);
  }
#endif
}





void
create_main_menu (void)
{
#if MENU_FEATURE
  if (! main_menu) main_menu = al_create_menu ();
  start_menu (main_menu, 0);

  menu_sub ("&Game", GAME_MID, true, NULL);

  menu_sub ("&View", VIEW_MID, true, NULL);

  menu_sub ("&Input", GAMEPAD_MID, true, NULL);

  menu_sub ("&Replay", REPLAY_MID, true, NULL);

  menu_sub ("&Editor", EDITOR_MID, can_edit (), NULL);

  menu_sub ("&Help", HELP_MID, true, NULL);

  end_menu ();

  game_menu ();
  view_menu ();
  gamepad_menu ();
  replay_menu ();
  editor_menu ();
  help_menu ();
#endif
}

ALLEGRO_BITMAP *
volume_icon (float volume)
{
  if (volume == VOLUME_RANGE_MIN) return micon (volume_off_icon);
  else if (volume < VOLUME_RANGE_LOW) return micon (volume_low_icon);
  else if (volume < VOLUME_RANGE_MEDIUM) return micon (volume_medium_icon);
  else if (volume <= VOLUME_RANGE_MAX) return micon (volume_high_icon);
  else assert (false);
  return NULL;
}

void
game_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, GAME_MID);

  menu_sub ("&Load", LOAD_MID, true, micon (open_icon));

  menu_sub ("&Save", SAVE_MID, true, micon (save_icon));

  menu_sep ();

  menu_citem ("&Mirror", MIRROR_MODE_MID, true,
              in_mirror_mode (), micon (horizontal_icon));

  menu_sub ("&Volume", VOLUME_MID, true, volume_icon (audio_volume));

  menu_sep ();

  menu_ditem (cutscene || title_demo,
              "Sta&rt (Enter)", "&Restart (Ctrl+R)",
              START_GAME_MID, RESTART_GAME_MID, true,
              micon (right_icon), micon (reload_icon));

  menu_sitem ("&Quit (Ctrl+Q)", QUIT_GAME_MID, true, micon (quit_icon));

  end_menu ();

  load_menu ();
  save_menu ();
  volume_menu ();
#endif
}

void
load_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, LOAD_MID);

  menu_sitem ("&Game... (Ctrl+L)", LOAD_GAME_MID, true, NULL);

  menu_sitem ("&Configuration... (Ctrl+L)", LOAD_CONFIG_MID, true, NULL);

  menu_sitem ("&Level file...", LOAD_LEVEL_FILE_MID, false, NULL);

  end_menu ();
#endif
}

void
save_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, SAVE_MID);

  menu_sitem ("&Game... (Ctrl+G)", SAVE_GAME_MID, true, NULL);

  menu_sitem ("&Configuration...", SAVE_CONFIG_MID, false, NULL);

  menu_sitem ("&Level file...", SAVE_LEVEL_FILE_MID, false, NULL);

  end_menu ();
#endif
}

void
volume_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, VOLUME_MID);

  menu_citem ("&Off", VOLUME_OFF_MID, true,
              audio_volume == VOLUME_RANGE_MIN,
              micon (volume_off_icon));

  menu_citem ("&Low", VOLUME_LOW_MID, true,
              audio_volume > VOLUME_RANGE_MIN
              && audio_volume < VOLUME_RANGE_LOW,
              micon (volume_low_icon));

  menu_citem ("&Medium", VOLUME_MEDIUM_MID, true,
              audio_volume >= VOLUME_RANGE_LOW
              && audio_volume < VOLUME_RANGE_MEDIUM,
              micon (volume_medium_icon));

  menu_citem ("&High", VOLUME_HIGH_MID, true,
              audio_volume >= VOLUME_RANGE_MEDIUM
              && audio_volume <= VOLUME_RANGE_MAX,
              micon (volume_high_icon));

  end_menu ();
#endif
}

void
view_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, VIEW_MID);

  menu_ditem (is_fullscreen (), "&Windowed (F)", "&Fullscreen (F)",
              FULL_SCREEN_MID, FULL_SCREEN_MID, true,
              micon (windows_icon), micon (full_screen_icon));

  menu_sub ("&Hue (F9)", HUE_MODE_MID, true, hue_icon (hue));

  menu_sub ("&Environment (F11)", ENVIRONMENT_MODE_MID, true, em_icon (em));

  menu_sub ("&Video (F12)", VIDEO_MODE_MID, true, vm_icon (vm));

  menu_sub ("&Flip (Shift+I)", FLIP_SCREEN_MID, true,
            micon_flags (screen_icon, screen_flags));

  menu_citem ("Inhibit screensaver", INHIBIT_SCREENSAVER_MID, true,
              inhibit_screensaver, micon (screensaver_icon));

  menu_sitem ("&Screenshot... (Ctrl+P)", SCREENSHOT_MID, true,
              micon (camera_icon));

  end_menu ();

  hue_mode_menu ();
  environment_mode_menu ();
  video_mode_menu ();
  screen_flip_menu ();
#endif
}

ALLEGRO_BITMAP *
hue_icon (enum hue hue)
{
  switch (hue) {
  case HUE_NONE: return micon (black_icon); break;
  case HUE_GREEN: return micon (green_icon); break;
  case HUE_GRAY: return micon (gray_icon); break;
  case HUE_YELLOW: return micon (yellow_icon); break;
  case HUE_BLUE: return micon (blue_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
hue_mode_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, HUE_MODE_MID);

  menu_citem ("&ORIGINAL", HUE_ORIGINAL_MID, true, ! force_hue,
              hue_icon (global_level.hue));

  menu_citem ("&NONE", HUE_NONE_MID, true, force_hue && hue == HUE_NONE,
              micon (black_icon));

  menu_citem ("&GREEN", HUE_GREEN_MID, true, force_hue && hue == HUE_GREEN,
              micon (green_icon));

  menu_citem ("&GRAY", HUE_GRAY_MID, true, force_hue && hue == HUE_GRAY,
              micon (gray_icon));

  menu_citem ("&YELLOW", HUE_YELLOW_MID, true, force_hue && hue == HUE_YELLOW,
              micon (yellow_icon));

  menu_citem ("&BLUE", HUE_BLUE_MID, true, force_hue && hue == HUE_BLUE,
              micon (blue_icon));

  end_menu ();
#endif
}

ALLEGRO_BITMAP *
em_icon (enum em em)
{
  switch (em) {
  case DUNGEON: return micon (dungeon_icon); break;
  case PALACE: return micon (palace_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
environment_mode_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, ENVIRONMENT_MODE_MID);

  menu_citem ("&ORIGINAL", ORIGINAL_ENV_MID, true, ! force_em,
              em_icon (global_level.em));

  menu_citem ("&DUNGEON", DUNGEON_MID, true, force_em && em == DUNGEON,
              micon (dungeon_icon));

  menu_citem ("&PALACE", PALACE_MID, true, force_em && em == PALACE,
              micon (palace_icon));

  end_menu ();
#endif
}

ALLEGRO_BITMAP *
vm_icon (enum vm vm)
{
  switch (vm) {
  case VGA: return micon (vga_icon); break;
  case EGA: return micon (ega_icon); break;
  case CGA: return micon (hgc ? hgc_icon : cga_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
video_mode_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, VIDEO_MODE_MID);

  menu_citem ("&VGA", VGA_MID, true, vm == VGA, micon (vga_icon));

  menu_citem ("&EGA", EGA_MID, true, vm == EGA, micon (ega_icon));

  menu_citem ("&CGA", CGA_MID, true, vm == CGA && ! hgc, micon (cga_icon));

  menu_citem ("&HGC", HGC_MID, true, vm == CGA && hgc, micon (hgc_icon));

  end_menu ();
#endif
}

void
screen_flip_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, FLIP_SCREEN_MID);

  menu_citem ("&Vertical", FLIP_SCREEN_VERTICAL_MID, true,
              screen_flags & ALLEGRO_FLIP_VERTICAL,
              micon (vertical_icon));

  menu_citem ("&Horizontal", FLIP_SCREEN_HORIZONTAL_MID, true,
              screen_flags & ALLEGRO_FLIP_HORIZONTAL,
              micon (horizontal_icon));

  end_menu ();
#endif
}

void
gamepad_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, GAMEPAD_MID);

  menu_sub ("&Device", GAMEPAD_DEVICE_MID, true, gamepad_device_icon (gpm));

  menu_sitem ("&Calibrate (Ctrl+J)", GAMEPAD_CALIBRATE_MID,
              gpm == JOYSTICK, micon (clock_icon));

  menu_sub ("&Flip (Shift+K)", FLIP_GAMEPAD_MID, true,
            micon_flags (joystick3_icon, bool2bitmap_flags
                         (flip_gamepad_vertical, flip_gamepad_horizontal)));

  end_menu ();

  gamepad_device_menu ();
  flip_gamepad_menu ();
#endif
}

ALLEGRO_BITMAP *
gamepad_device_icon (enum gpm gpm)
{
  switch (gpm) {
  case KEYBOARD: return micon (keyboard_icon); break;
  case JOYSTICK: return micon (joystick_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
gamepad_device_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, GAMEPAD_DEVICE_MID);

  menu_citem ("&Keyboard (Ctrl+K)", KEYBOARD_MODE_MID, true,
              gpm == KEYBOARD, micon (keyboard_icon));

  menu_citem ("&Joystick (Ctrl+J)", JOYSTICK_MODE_MID, joystick,
              gpm == JOYSTICK, micon (joystick_icon));

  end_menu ();
#endif
}

void
flip_gamepad_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, FLIP_GAMEPAD_MID);

  menu_citem ("&Vertical", FLIP_GAMEPAD_VERTICAL_MID, true,
              flip_gamepad_vertical, micon (vertical_icon));

  menu_citem ("&Horizontal", FLIP_GAMEPAD_HORIZONTAL_MID, true,
              flip_gamepad_horizontal, micon (horizontal_icon));

  end_menu ();
#endif
}

void
replay_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, REPLAY_MID);

  char *text;

  if (recording_replay_countdown >= 0
      || level_start_replay_mode == RECORD_REPLAY)
    goto record_replay;

  if (title_demo) goto no_replay;

  switch (replay_mode) {
  case PLAY_REPLAY:
    xasprintf (&text, "REPLAYING (%i/%i)", replay_index + 1,
               replay_chain_nmemb);
    menu_sitem (text, NO_MID, false, micon (play_icon));
    al_free (text);

    menu_sep ();

    menu_sitem
      ("&Stop (F7)", PLAY_REPLAY_MID, true, micon (stop_icon));

    menu_sitem
      ("Pre&vious (Shift+M)", PREVIOUS_REPLAY_MID,
       replay_index > 0, micon (previous_icon));

    menu_sitem
      ("&Next (Shift+L)", NEXT_REPLAY_MID,
       replay_index + 1 < replay_chain_nmemb, micon (next_icon));

    menu_widget_pause ();

    menu_widget_speed ();

    break;
  case RECORD_REPLAY: record_replay:

    menu_sitem ("RECORDING", NO_MID, false, micon (record_icon));

    menu_sep ();

    menu_sitem
      (recording_replay_countdown >= 0
       ? "&Abort recording (F7)" : "&Stop... (F7)",
       RECORD_REPLAY_MID, true,
       replay_mode == RECORD_REPLAY
       ? micon (stop_icon) : micon (eject_icon));

    menu_widget_pause ();

    break;
  default: assert (false);
  case NO_REPLAY: no_replay:

    menu_sitem
      ("&Play... (F7)", PLAY_REPLAY_MID, true, micon (play_icon));

    menu_sitem
      ("&Record... (Alt+F7)", RECORD_REPLAY_MID, true,
       micon (record_icon));

    menu_widget_pause ();

    menu_widget_speed ();

    break;
  }

  end_menu ();
#endif
}

void
editor_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, EDITOR_MID);

  menu_ditem (edit == EDIT_NONE, "&Edit (F8)", "&Play (F8)",
              EDIT_MODE_MID, EDIT_MODE_MID,
              can_edit (),
              micon (edit_icon), micon (joystick2_icon));

  menu_sitem ("&Undo (Ctrl+Z)", UNDO_MID,
              can_edit () && can_undo (&undo, -1),
              micon (undo_icon));

  menu_sitem ("&Redo (Ctrl+Y)", REDO_MID,
              can_edit () && can_undo (&undo, +1),
              micon (redo_icon));

  end_menu ();
#endif
}

void
help_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, HELP_MID);

  menu_sitem ("&About", ABOUT_MID, true, micon (small_logo_icon));

  end_menu ();
#endif
}

#if MENU_FEATURE
void
menu_widget_pause (void)
{
  menu_sep ();

  menu_sitem
    (is_game_paused ()
     ? "&Continue" : "&Pause (Esc)", TOGGLE_PAUSE_GAME_MID,
     ! cutscene && ! title_demo && recording_replay_countdown < 0,
     is_game_paused () ? micon (play_icon) : micon (pause_icon));

  menu_sitem
    ("Next &frame (Esc)", NEXT_FRAME_MID,
     is_game_paused () && ! cutscene && ! title_demo
     && recording_replay_countdown < 0,
     micon (forward_icon));
}
#endif

#if MENU_FEATURE
void
menu_widget_speed (void)
{
  menu_sep ();

  menu_citem
    ("Speed cons&traint", TOGGLE_TIME_FREQUENCY_CONSTRAINT_MID,
     ! cutscene && ! title_demo, anim_freq > 0, micon (cancel_icon));

  menu_sitem
    ("&Decrease speed [(]", DECREASE_TIME_FREQUENCY_MID,
     anim_freq > 2 && ! cutscene && ! title_demo,
     micon (backward_icon));

  menu_sitem
    ("&Increase speed [)]", INCREASE_TIME_FREQUENCY_MID,
     anim_freq != 0 && ! cutscene && ! title_demo,
     micon (forward_icon));
}
#endif






void
menu_mid (intptr_t mid)
{
  /********/
  /* MENU */
  /********/

  switch (mid) {

    /* GAME */
  case LOAD_GAME_MID:
    ui_load_game ();
    break;
  case LOAD_CONFIG_MID:
    ui_load_config ();
    break;
  case MIRROR_MODE_MID:
    ui_mirror_mode (! in_mirror_mode ());
    break;
  case VOLUME_OFF_MID:
    ui_audio_volume (VOLUME_OFF);
    break;
  case VOLUME_LOW_MID:
    ui_audio_volume (VOLUME_LOW);
    break;
  case VOLUME_MEDIUM_MID:
    ui_audio_volume (VOLUME_MEDIUM);
    break;
  case VOLUME_HIGH_MID:
    ui_audio_volume (VOLUME_HIGH);
    break;
  case RESTART_GAME_MID:
    ui_restart_game ();
    break;
  case START_GAME_MID:
    ui_start_game ();
    break;
  case QUIT_GAME_MID:
    ui_quit_game ();
    break;


    /* VIEW */
  case FULL_SCREEN_MID:
    ui_full_screen ();
    break;
  case HUE_ORIGINAL_MID:
    ui_hue_mode (HUE_ORIGINAL);
    break;
  case HUE_NONE_MID:
    ui_hue_mode (HUE_NONE);
    break;
  case HUE_GREEN_MID:
    ui_hue_mode (HUE_GREEN);
    break;
  case HUE_GRAY_MID:
    ui_hue_mode (HUE_GRAY);
    break;
  case HUE_YELLOW_MID:
    ui_hue_mode (HUE_YELLOW);
    break;
  case HUE_BLUE_MID:
    ui_hue_mode (HUE_BLUE);
    break;
  case ORIGINAL_ENV_MID:
    ui_environment_mode (ORIGINAL_ENV);
    break;
  case DUNGEON_MID:
    ui_environment_mode (DUNGEON);
    break;
  case PALACE_MID:
    ui_environment_mode (PALACE);
    break;
  case VGA_MID:
    ui_video_mode (VGA);
    break;
  case EGA_MID:
    ui_video_mode (EGA);
    break;
  case CGA_MID:
    ui_video_mode (CGA);
    break;
  case HGC_MID:
    ui_video_mode (HGC);
    break;
  case FLIP_SCREEN_VERTICAL_MID:
    ui_flip_screen (screen_flags ^ ALLEGRO_FLIP_VERTICAL, false, false);
    break;
  case FLIP_SCREEN_HORIZONTAL_MID:
    ui_flip_screen (screen_flags ^ ALLEGRO_FLIP_HORIZONTAL, false, false);
    break;
  case INHIBIT_SCREENSAVER_MID:
    ui_inhibit_screensaver (! inhibit_screensaver);
    break;
  case SCREENSHOT_MID:
    ui_screenshot ();
    break;


    /* GAMEPAD */
  case KEYBOARD_MODE_MID:
    ui_gamepad_mode (KEYBOARD);
    break;
  case JOYSTICK_MODE_MID:
    ui_gamepad_mode (JOYSTICK);
    break;
  case GAMEPAD_CALIBRATE_MID:
    ui_gamepad_mode (JOYSTICK);
    break;
  case FLIP_GAMEPAD_VERTICAL_MID:
    ui_flip_gamepad (! flip_gamepad_vertical, flip_gamepad_horizontal, false);
    break;
  case FLIP_GAMEPAD_HORIZONTAL_MID:
    ui_flip_gamepad (flip_gamepad_vertical, ! flip_gamepad_horizontal, false);
    break;


    /* REPLAY */
  case PLAY_REPLAY_MID:
    ui_play_replay ();
    break;
  case PREVIOUS_REPLAY_MID:
    ui_previous_replay ();
    break;
  case NEXT_REPLAY_MID:
    ui_next_replay ();
    break;
  case RECORD_REPLAY_MID:
    ui_record_replay ();
    break;
  case TOGGLE_PAUSE_GAME_MID:
    ui_toggle_pause_game ();
    break;
  case NEXT_FRAME_MID:
    ui_next_frame ();
    break;
  case DECREASE_TIME_FREQUENCY_MID:
    ui_decrease_time_frequency ();
    break;
  case INCREASE_TIME_FREQUENCY_MID:
    ui_increase_time_frequency ();
    break;
  case TOGGLE_TIME_FREQUENCY_CONSTRAINT_MID:
    ui_toggle_time_frequency_constraint ();
    break;


    /* EDITOR */
  case EDIT_MODE_MID:
    ui_editor ();
    break;
  case UNDO_MID:
    ui_undo_pass (&undo, -1, NULL);
    break;
  case REDO_MID:
    ui_undo_pass (&undo, +1, NULL);
    break;


    /* HELP */
  case ABOUT_MID:
    ui_about_screen (true);
    break;
  default: break;
  }
}






void
anim_key_bindings (void)
{
  /****************/
  /* KEY BINDINGS */
  /****************/


  /* CTRL+L: load game */
  if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_L)
      && ! load_config_dialog_thread)
    ui_load_game ();

  /* CTRL+P: screenshot */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_P)
           && ! save_picture_dialog_thread)
    ui_screenshot ();

  /* ALT+F7: start/stop replay recording */
  else if (! command_line_replay
           && (((title_demo || replay_mode != PLAY_REPLAY)
                && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F7))
               || ((replay_mode == RECORD_REPLAY
                    || recording_replay_countdown > 0)
                   && was_key_pressed (0, ALLEGRO_KEY_F7))))
    ui_record_replay ();

  /* F7: load replay/stop replaying */
  else if (! command_line_replay
           && ((replay_mode != RECORD_REPLAY
                && was_key_pressed (0, ALLEGRO_KEY_F7))
               || (replay_mode == PLAY_REPLAY
                   && was_key_pressed
                   (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F7))))
    ui_play_replay ();

  /* CTRL+R: restart game */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_R))
    ui_restart_game ();

  /* CTRL+Q: quit game */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_Q))
    ui_quit_game ();

  /* (: decrease time frequency */
  else if (was_char_pressed ('('))
    ui_change_anim_freq (anim_freq - 1);

  /* ): increase time frenquency */
  else if (was_char_pressed (')'))
    ui_change_anim_freq (anim_freq + 1);

  /* F8: enable/disable level editor */
  else if (was_key_pressed (0, ALLEGRO_KEY_F8))
    ui_editor ();

  /* CTRL+V: show engine name and version */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_V))
    ui_version ();

  /* CTRL+S: enable/disable sound */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_S)) {
    if (audio_volume == VOLUME_RANGE_MIN) ui_audio_volume (VOLUME_LOW);
    else if (audio_volume < VOLUME_RANGE_LOW) ui_audio_volume (VOLUME_MEDIUM);
    else if (audio_volume < VOLUME_RANGE_MEDIUM) ui_audio_volume (VOLUME_HIGH);
    else if (audio_volume <= VOLUME_RANGE_MAX) ui_audio_volume (VOLUME_OFF);
  }

  /* F: enable/disable fullscreen mode */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_F))
           || was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_ENTER))
    ui_full_screen ();

  /* SHIFT+I: flip screen */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_I)) {
    switch (screen_flags) {
    case 0: ui_flip_screen (ALLEGRO_FLIP_VERTICAL, true, false); break;
    case ALLEGRO_FLIP_VERTICAL:
      ui_flip_screen (ALLEGRO_FLIP_HORIZONTAL, true, false); break;
    case ALLEGRO_FLIP_HORIZONTAL:
      ui_flip_screen (ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL,
                      true, false);
      break;
    case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    default: ui_flip_screen (0, true, false); break;
    }
  }

  /* SHIFT+K: flip gamepad */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_K)) {
    if (! flip_gamepad_vertical && ! flip_gamepad_horizontal)
      ui_flip_gamepad (true, false, false);
    else if (flip_gamepad_vertical && ! flip_gamepad_horizontal)
      ui_flip_gamepad (false, true, false);
    else if (! flip_gamepad_vertical && flip_gamepad_horizontal)
      ui_flip_gamepad (true, true, false);
    else if (flip_gamepad_vertical && flip_gamepad_horizontal)
      ui_flip_gamepad (false, false, false);
  }

  /* CTRL+K: keyboard mode (disables joystick) */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_K))
    ui_gamepad_mode (KEYBOARD);

  /* CTRL+J: joystick mode */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_J))
    ui_gamepad_mode (JOYSTICK);

  /* F9: change hue palette */
  else if (was_key_pressed (0, ALLEGRO_KEY_F9)) {
    if (force_hue)
      switch (hue) {
      case HUE_NONE: ui_hue_mode (HUE_GREEN); break;
      case HUE_GREEN: ui_hue_mode (HUE_GRAY); break;
      case HUE_GRAY: ui_hue_mode (HUE_YELLOW); break;
      case HUE_YELLOW: ui_hue_mode (HUE_BLUE); break;
      case HUE_BLUE: ui_hue_mode (HUE_ORIGINAL); break;
      }
    else ui_hue_mode (HUE_NONE);
  }

  /* F11: change environment mode */
  else if (was_key_pressed (0, ALLEGRO_KEY_F11)) {
    if (force_em) {
      switch (em) {
      case DUNGEON: ui_environment_mode (PALACE); break;
      case PALACE: ui_environment_mode (ORIGINAL_ENV); break;
      }
    } else ui_environment_mode (DUNGEON);
  }

  /* F12: change video mode */
  else if (was_key_pressed (0, ALLEGRO_KEY_F12))
    switch (vm) {
    case VGA: ui_video_mode (EGA); break;
    case EGA: ui_video_mode (CGA); break;
    case CGA: ui_video_mode (hgc ? VGA : HGC); break;
    }

  /* D: change display mode */
  else if (! active_menu && was_key_pressed (0, ALLEGRO_KEY_D))
    ui_next_display_mode ();
}






void
ui_editor (void)
{
  if (edit == EDIT_NONE) {
    if (cutscene) toggle_menu_visibility ();
    else {
      if (replay_mode == NO_REPLAY ) {
        enter_editor ();
        show_menu ();
      } else  {
        toggle_menu_visibility ();
        print_replay_mode (0);
      }
    }
  } else {
    exit_editor (0);
    hide_menu ();
  }

  editor_menu ();
}

void
ui_load_game (void)
{
  load_config_dialog_thread =
    create_thread (dialog_thread, &load_config_dialog);
  al_start_thread (load_config_dialog_thread);
}

void
ui_load_config (void)
{
  load_config_dialog_thread =
    create_thread (dialog_thread, &load_config_dialog);
  al_start_thread (load_config_dialog_thread);
}

void
ui_audio_volume (float volume)
{
  char *key = "SOUND GAIN";
  char *value;
  xasprintf (&value, "%.1f", volume);

  set_audio_volume (volume);

  char *text;
  char *status;
  if (volume == VOLUME_RANGE_MIN) status = "OFF";
  else if (volume < VOLUME_RANGE_LOW) status = "LOW";
  else if (volume < VOLUME_RANGE_MEDIUM) status = "MEDIUM";
  else if (volume <= VOLUME_RANGE_MAX) status = "HIGH";

  xasprintf (&text, "SOUND %s", status);
  draw_bottom_text (NULL, text, 0);
  al_free (text);

  ui_save_setting (key, value);
  al_free (value);
  game_menu ();
}

void
ui_screenshot (void)
{
  save_picture_dialog_thread =
    create_thread (dialog_thread, &save_picture_dialog);
  al_start_thread (save_picture_dialog_thread);
  pause_animation (true);
}

void
ui_restart_game (void)
{
  quit_anim = RESTART_GAME;
}

void
ui_start_game (void)
{
  quit_anim = CUTSCENE_KEY_PRESS;
}

void
ui_quit_game (void)
{
  quit_anim = QUIT_GAME;
}

void
ui_full_screen (void)
{
  if (display_mode < 0) {
    char *key = "FULLSCREEN";
    char *value;
    force_full_redraw = true;
    if (is_fullscreen ()) {
      al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, false);
      value = "OFF";
      show_mouse_cursor ();
      show_menu ();
    } else {
      al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, true);
      value = "ON";
      if (edit == EDIT_NONE) hide_menu ();
    }
    char *text;
    xasprintf (&text, "%s: %s", key, value);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
    ui_save_setting (key, value);
    view_menu ();
  } else draw_bottom_text (NULL, "NON-DESKTOP MODE IS FULLSCREEN ONLY", 0);
}

void
ui_hue_mode (enum hue new_hue)
{
  char *key = "HUE MODE";
  char *value = NULL;

  switch (new_hue) {
  default: /* HUE_ORIGINAL */
    force_hue = false;
    hue = global_level.hue;
    value = "ORIGINAL";
    break;
  case HUE_NONE:
    force_hue = true;
    hue = HUE_NONE;
    value = "NONE";
    break;
  case HUE_GREEN:
    force_hue = true;
    hue = HUE_GREEN;
    value = "GREEN";
    break;
  case HUE_GRAY:
    force_hue = true;
    hue = HUE_GRAY;
    value = "GRAY";
    break;
  case HUE_YELLOW:
    force_hue = true;
    hue = HUE_YELLOW;
    value = "YELLOW";
    break;
  case HUE_BLUE:
    force_hue = true;
    hue = HUE_BLUE;
    value = "BLUE";
    break;
  }

  char *text;
  xasprintf (&text, "%s: %s", key, value);
  draw_bottom_text (NULL, text, 0);
  al_free (text);

  ui_save_setting (key, value);
  view_menu ();
}

void
ui_environment_mode (enum em new_em)
{
  char *key = "ENVIRONMENT MODE";
  char *value = NULL;

  switch (new_em) {
  default: /* ORIGINAL_ENV */
    force_em = false;
    em = global_level.em;
    value = "ORIGINAL";
    break;
  case DUNGEON:
    force_em = true;
    em = DUNGEON;
    value = "DUNGEON";
    break;
  case PALACE:
    force_em = true;
    em = PALACE;
    value = "PALACE";
    break;
  }

  char *text;
  xasprintf (&text, "%s: %s", key, value);
  draw_bottom_text (NULL, text, 0);
  al_free (text);

  ui_save_setting (key, value);
  view_menu ();
}

void
ui_video_mode (enum vm new_vm)
{
  char *key = "VIDEO MODE";
  char *value;

  if (new_vm == HGC) {
    vm = CGA; hgc = true;
  } else {
    vm = new_vm; hgc = false;
  }

  switch (vm) {
  case VGA: value = "VGA"; break;
  case EGA: value = "EGA"; break;
  case CGA: value = hgc ? "HGC" : "CGA"; break;
  }

  video_mode_menu ();

  char *text;
  xasprintf (&text, "%s: %s", key, value);
  draw_bottom_text (NULL, text, 0);
  al_free (text);

  ui_save_setting (key, value);
  view_menu ();
}

void
ui_flip_screen (int flags, bool correct_mouse, bool save_only)
{
  char *key = "DISPLAY FLIP MODE";
  char *value;

  switch (flags) {
  case 0: value = "NONE"; break;
  case ALLEGRO_FLIP_VERTICAL: value = "VERTICAL"; break;
  case ALLEGRO_FLIP_HORIZONTAL: value = "HORIZONTAL"; break;
  case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    value = "VERTICAL-HORIZONTAL"; break;
  default: assert (false); break;
  }

  ui_save_setting (key, value);

  if (save_only) return;

  if (correct_mouse) {
    int w = al_get_display_width (display);
    int h = al_get_display_height (display);

    ALLEGRO_MOUSE_STATE m;

    if ((screen_flags & ALLEGRO_FLIP_VERTICAL
         && ! (flags & ALLEGRO_FLIP_VERTICAL))
        || (! (screen_flags & ALLEGRO_FLIP_VERTICAL)
            && flags & ALLEGRO_FLIP_VERTICAL)) {
      al_get_mouse_state (&m);
      al_set_mouse_xy (display, m.x, h - m.y);
    }

    if ((screen_flags & ALLEGRO_FLIP_HORIZONTAL
         && ! (flags & ALLEGRO_FLIP_HORIZONTAL))
        || (! (screen_flags & ALLEGRO_FLIP_HORIZONTAL)
            && flags & ALLEGRO_FLIP_HORIZONTAL)) {
      al_get_mouse_state (&m);
      al_set_mouse_xy (display, w - m.x, m.y);
    }
  }

  potion_flags = 0;
  screen_flags = flags;

  char *text;
  xasprintf (&text, "%s: %s", key, value);
  draw_bottom_text (NULL, text, 1);
  al_free (text);

  ui_flip_gamepad (flip_gamepad_vertical, flip_gamepad_horizontal, true);
  ui_save_setting ("MIRROR MODE", NULL);
  game_menu ();
  view_menu ();
}

void
ui_inhibit_screensaver (bool inhibit)
{
  char *key = "INHIBIT SCREENSAVER";
  char *value = inhibit ? "ON" : "OFF";

  inhibit_screensaver = inhibit;
  al_inhibit_screensaver (inhibit_screensaver);

  char *text;
  xasprintf (&text, "%s: %s", key, value);
  draw_bottom_text (NULL, text, 1);
  al_free (text);

  ui_save_setting (key, value);
  view_menu ();
}

void
ui_gamepad_mode (enum gpm new_gpm)
{
  char *key = "GAMEPAD MODE";
  char *value;

  char *text;

  switch (new_gpm) {
  case KEYBOARD:
    gpm = KEYBOARD;
    draw_bottom_text (NULL, "KEYBOARD MODE", 0);
    value = "KEYBOARD";
    ui_save_setting (key, value);
    break;
  case JOYSTICK:
    if (calibrate_joystick ()) {
      xasprintf (&text, "JOYSTICK %s",
                 gpm == JOYSTICK ? "CALIBRATED" : "MODE");
      gpm = JOYSTICK;
      value = "JOYSTICK";
      ui_save_setting (key, value);
      gamepad_rumble (1.0, 0.6);
    } else xasprintf (&text, "JOYSTICK NOT FOUND");
    draw_bottom_text (NULL, text, 0);
    al_free (text);
    break;
  default: assert (false);
  }

  gamepad_menu ();
}

void
ui_flip_gamepad (bool v, bool h, bool save_only)
{
  char *key = "GAMEPAD FLIP MODE";
  char *value;

  if (! v && ! h) value = "NONE";
  else if (v && ! h) value = "VERTICAL";
  else if (! v && h) value = "HORIZONTAL";
  else if (v && h) value = "VERTICAL-HORIZONTAL";

  ui_save_setting (key, value);

  if (save_only) return;

  flip_gamepad_vertical = v;
  flip_gamepad_horizontal = h;

  char *text;
  xasprintf (&text, "%s: %s", key, value);
  draw_bottom_text (NULL, text, 0);
  al_free (text);

  ui_flip_screen (screen_flags, false, true);
  ui_save_setting ("MIRROR MODE", NULL);
  game_menu ();
  gamepad_menu ();
}

void
ui_play_replay (void)
{
  if (! title_demo && replay_mode == PLAY_REPLAY) {
    print_replay_chain_aborted ();
    stop_replaying (2);
  } else create_load_replay_thread ();
}

void
ui_previous_replay (void)
{
  quit_anim = REPLAY_PREVIOUS;
}

void
ui_next_replay (void)
{
  quit_anim = REPLAY_NEXT;
}

void
ui_record_replay (void)
{
  if (replay_mode == RECORD_REPLAY) create_save_replay_thread ();
  else if (recording_replay_countdown < 0)
    prepare_for_recording_replay ();
  else {
    draw_bottom_text (NULL, "RECORDING ABORTED", 2);
    recording_replay_countdown = -1;
    replay_menu ();
  }
}

void
ui_increase_time_frequency (void)
{
  if (anim_freq >= DEFAULT_HZ)
    ui_change_anim_freq (DEFAULT_HZ * ((anim_freq / DEFAULT_HZ) + 1));
  else ui_change_anim_freq (2 * ((anim_freq / 2) + 1));
}

void
ui_decrease_time_frequency (void)
{
  if (anim_freq > DEFAULT_HZ)
    ui_change_anim_freq (DEFAULT_HZ * (anim_freq / DEFAULT_HZ - 1));
  else if (anim_freq > 2)
    ui_change_anim_freq (2 * ((anim_freq / 2) - 1));
}

void
ui_toggle_time_frequency_constraint (void)
{
  static int f = DEFAULT_HZ;

  if (anim_freq > 0) {
    f = anim_freq;
    ui_change_anim_freq (0);
  } else ui_change_anim_freq (f);
}

void
ui_change_anim_freq (int f)
{
  char *text;
  f = f < 0 ? 0 : f;
  f = f > UNLIMITED_HZ ? UNLIMITED_HZ : f;
  anim_freq = f;
  al_set_timer_speed (timer, f > 0 ? 1.0 / f : 1.0 / UNLIMITED_HZ);
  if (f > 0) {
    xasprintf (&text, "TIME FREQ: %iHz", f);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }
  replay_menu ();
}

void
ui_toggle_pause_game (void)
{
  if (is_game_paused ()) pause_game (false);
  else pause_game (true);
  replay_menu ();
}

void
ui_next_frame (void)
{
  step_cycle = 1;
}

void
print_game_paused (int priority)
{
  char *text;

  switch (replay_mode) {
  case RECORD_REPLAY: text = "RECORDING PAUSED"; break;
  case PLAY_REPLAY: text = "REPLAY PAUSED"; break;
  case NO_REPLAY: default: text = "GAME PAUSED"; break;
  }

  draw_bottom_text (NULL, text, priority);
}

void
ui_undo_pass (struct undo *u, int dir, char *prefix)
{
  if (replay_mode != NO_REPLAY) {
    print_replay_mode (0);
    return;
  }

  char *text;
  char *dir_str = (dir >= 0) ? "REDO" : "UNDO";
  static char *undo_msg = NULL;

  bool b = can_undo (u, dir);

  if (undo_msg) al_free (undo_msg);

  if (! b) {
    if (prefix) xasprintf (&undo_msg, "NO FURTHER %s %s", prefix, dir_str);
    else xasprintf (&undo_msg, "NO FURTHER %s", dir_str);
    editor_msg (undo_msg, EDITOR_CYCLES_3);
    return;
  }

  undo_pass (u, dir, &text);

  if (prefix) xasprintf (&undo_msg, "%s %s: %s", prefix, dir_str, text);
  else xasprintf (&undo_msg, "%s: %s", dir_str, text);
  editor_msg (undo_msg, EDITOR_CYCLES_3);
}

void
ui_version (void)
{
  char *text;
  xasprintf (&text, "MININIM %s", VERSION);
  draw_bottom_text (NULL, text, 0);
  al_free (text);
}

void
ui_about_screen (bool value)
{
  if (value) {
    enable_menu (false);
    if (! message_box_thread_id) {
      message_box_thread_id =
        create_thread (message_box_thread, &about_dialog);
      al_start_thread (message_box_thread_id);
    }
    about_screen = true;
    pause_animation (true);
  } else if (! message_box_thread_id) {
    about_screen = false;
    pause_animation (false);
    create_main_menu ();
  }
}

void
ui_next_display_mode (void)
{
  char *key = "DISPLAY MODE";
  char *value;

  if (display_mode < 0)
    draw_bottom_text (NULL, "DISPLAY MODE: DESKTOP", 0);
  else {
    int n = al_get_num_display_modes ();
    if (n) {
      int display_mode_bkp = display_mode;
    next_display_mode:
      display_mode = (display_mode + 1) % n;
      int w = al_get_display_width (display);
      int h = al_get_display_height (display);
      ALLEGRO_DISPLAY_MODE d;
      if (al_get_display_mode (display_mode, &d)) {
        if (d.width == w && d.height == h
            && display_mode != display_mode_bkp)
          goto next_display_mode;
        char *text;
        al_resize_display (display, d.width, d.height);
        xasprintf (&text, "%s: %ix%i", key, d.width, d.height);
        draw_bottom_text (NULL, text, 0);
        al_free (text);
        xasprintf (&value, "%i", display_mode);
        ui_save_setting (key, value);
        al_free (value);
      } else draw_bottom_text (NULL, "DISPLAY MODES QUERYING FAILED", 0);
    } else draw_bottom_text (NULL, "NO DISPLAY MODE AVAILABLE", 0);
  }
}





error_t
ui_save_setting (char *key, char *value)
{
  ALLEGRO_CONFIG *config = NULL;

  if (al_filename_exists (config_filename))
    config = al_load_config_file (config_filename);
  else if (value) {
    if (! al_make_directory (user_settings_dir))
      return al_get_errno ();
    config = al_create_config ();
  } else return 0;

  if (! config) return al_get_errno ();

  ALLEGRO_CONFIG_ENTRY *iterator;
  char const *entry = al_get_first_config_entry (config, NULL, &iterator);

  while (entry) {
    if (! strcasecmp (entry, key)) {
      al_remove_config_key (config, NULL, entry);
      entry = al_get_first_config_entry (config, NULL, &iterator);
    } else entry = al_get_next_config_entry (&iterator);
  }

  if (value) al_set_config_value (config, NULL, key, value);

  if (! al_save_config_file (config_filename, config)) {
    al_destroy_config (config);
    return al_get_errno ();
  }

  al_destroy_config (config);

  return 0;
}





bool
in_mirror_mode (void)
{
  return ! flip_gamepad_vertical && flip_gamepad_horizontal
    && screen_flags == ALLEGRO_FLIP_HORIZONTAL;
}

void
mirror_mode (bool mirror)
{
  if (mirror) {
    flip_gamepad_vertical = false;
    flip_gamepad_horizontal = true;
    screen_flags = ALLEGRO_FLIP_HORIZONTAL;
  } else {
    flip_gamepad_vertical = false;
    flip_gamepad_horizontal = false;
    screen_flags = 0;
  }
}

void
ui_mirror_mode (bool mirror)
{
  char *key = "MIRROR MODE";
  char *value = mirror ? "ON" : "OFF";

  mirror_mode (mirror);

  char *text;
  xasprintf (&text, "%s: %s", key, value);
  draw_bottom_text (NULL, text, 0);
  al_free (text);

  ui_save_setting ("DISPLAY FLIP MODE", NULL);
  ui_save_setting ("GAMEPAD FLIP MODE", NULL);
  ui_save_setting (key, value);
  game_menu ();
  view_menu ();
  gamepad_menu ();
}
