/*
  video.h -- video module;

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

#ifndef MININIM_VIDEO_H
#define MININIM_VIDEO_H

/* functions */
void init_video (void);
void finalize_video (void);
void destroy_bitmap (ALLEGRO_BITMAP *bitmap);
int memory_bitmap_flags (void);
int video_bitmap_flags (void);
ALLEGRO_BITMAP *create_memory_bitmap (int w, int h);
ALLEGRO_BITMAP *create_bitmap (int w, int h);
ALLEGRO_BITMAP *clone_bitmap (ALLEGRO_BITMAP *bitmap);
ALLEGRO_BITMAP *load_memory_bitmap (char *filename);
ALLEGRO_BITMAP *clone_memory_bitmap (ALLEGRO_BITMAP *bitmap);
ALLEGRO_BITMAP *load_scaled_memory_bitmap (char *filename, int w, int h,
                                           int flags);
ALLEGRO_BITMAP *clone_scaled_memory_bitmap (ALLEGRO_BITMAP *bitmap, int w,
                                            int h, int flags);
ALLEGRO_BITMAP *load_bitmap (const char *filename);
void validate_bitmap_for_mingw (ALLEGRO_BITMAP *bitmap);
void save_bitmap (char *filename, ALLEGRO_BITMAP *bitmap);
bool bitmap_heq (ALLEGRO_BITMAP *b0, ALLEGRO_BITMAP *b1);
bool color_eq (ALLEGRO_COLOR c0, ALLEGRO_COLOR c1);
void set_target_bitmap (ALLEGRO_BITMAP *bitmap);
void set_target_backbuffer (ALLEGRO_DISPLAY *display);
void clear_bitmap (ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR color);
void draw_bitmap (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                  float dx, float dy, int flags);
void draw_bitmap_region (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                         float sx, float sy, float sw, float sh,
                         float dx, float dy, int flags);
void draw_filled_rectangle (ALLEGRO_BITMAP *to, float x1, float y1,
                            float x2, float y2, ALLEGRO_COLOR color);
void draw_rectangle (ALLEGRO_BITMAP *to, float x1, float y1,
                     float x2, float y2, ALLEGRO_COLOR color,
                     float thickness);
void draw_text (ALLEGRO_BITMAP *bitmap, char const *text, float x, float y, int flags);
bool draw_bottom_text (ALLEGRO_BITMAP *bitmap, char *text, int priority);
void draw_fade (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to, float factor);
void draw_roll_right (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                      int total, int current);
void draw_shutter (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                   int total, int i);
void draw_pattern (ALLEGRO_BITMAP *bitmap, int ox, int oy, int w, int h,
                   ALLEGRO_COLOR color_0, ALLEGRO_COLOR color_1);
void start_video_effect (enum video_effect_type type, int duration);
void stop_video_effect (void);
bool is_video_effect_started (void);
void show (void);
void flip_display (ALLEGRO_BITMAP *bitmap);
bool is_fullscreen (void);
void process_display_events (void);
int bool2bitmap_flags (bool v, bool h);

/* special screens */
void draw_logo (ALLEGRO_BITMAP *bitmap, char *text0, char *text1,
                ALLEGRO_BITMAP *icon);
void show_logo (char *text0, char* text1, ALLEGRO_BITMAP *icon);
void show_logo_replaying (void);
void load_oitofelix_face (void);
void unload_oitofelix_face (void);
ALLEGRO_BITMAP *oitofelix_face (enum vm vm);

/* palette */
int compare_palette_caches (const void *pc0, const void *pc1);
void palette_cache_gc (lua_State *L);
ALLEGRO_BITMAP *apply_palette (ALLEGRO_BITMAP *bitmap, palette p);
ALLEGRO_BITMAP *apply_palette_k (ALLEGRO_BITMAP *bitmap, palette p,
                                 const void *k);
ALLEGRO_BITMAP *get_cached_palette (ALLEGRO_BITMAP *bitmap, palette p);
ALLEGRO_COLOR hgc_palette (ALLEGRO_COLOR c);

/* drawn and clipping rectangle */
bool intersection_rectangle (int x0, int y0, int w0, int h0,
                             int x1, int y1, int w1, int h1,
                             int *xrp, int *yrp, int *wrp, int *hrp);
bool union_rectangle (int x0, int y0, int w0, int h0,
                      int x1, int y1, int w1, int h1,
                      int *xrp, int *yrp, int *wrp, int *hrp);

struct drawn_rectangle *push_drawn_rectangle (ALLEGRO_BITMAP *bitmap);
struct drawn_rectangle *get_drawn_rectangle (ALLEGRO_BITMAP *bitmap);
struct drawn_rectangle *merge_drawn_rectangle (ALLEGRO_BITMAP *bitmap,
                                               int x, int y, int w, int h);
struct drawn_rectangle *pop_drawn_rectangle (void);
void adjust_drawn_rectangle (ALLEGRO_BITMAP *bitmap, int dx, int dy,
                             int dw, int dh);

void push_clipping_rectangle (ALLEGRO_BITMAP *bitmap,
                              int x, int y, int w, int h);
void push_reset_clipping_rectangle (ALLEGRO_BITMAP *bitmap);
bool merge_clipping_rectangle (ALLEGRO_BITMAP *bitmap, int x, int y,
                               int w, int h);
void pop_clipping_rectangle (void);

/* variables */
extern bool force_full_redraw;
extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_BITMAP *cutscene_screen;
extern ALLEGRO_BITMAP *uscreen, *iscreen;
extern ALLEGRO_BITMAP *effect_buffer;
extern ALLEGRO_BITMAP *black_screen;
extern ALLEGRO_TIMER *video_timer;
extern struct video_effect video_effect;
extern int screen_flags, potion_flags;
extern bool hgc;
extern int display_width, display_height;
extern ALLEGRO_BITMAP *logo_icon;
extern uint64_t bottom_text_timer;
extern int effect_counter;
extern void (*load_callback) (void);
extern int display_mode;
extern bool ignore_clipping_rectangle_intersection;
extern bool about_screen;
extern ALLEGRO_BITMAP *oitofelix_face_gray, *oitofelix_face_bw;
extern ssize_t palette_cache_size_limit;

struct drawn_rectangle drawn_rectangle_stack[DRAWN_RECTANGLE_STACK_NMEMB_MAX];
size_t drawn_rectangle_stack_nmemb;

#endif	/* MININIM_VIDEO_H */
