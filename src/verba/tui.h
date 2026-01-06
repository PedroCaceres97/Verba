#ifndef __VERBA_TUI_H__
#define __VERBA_TUI_H__

#include <verba/mem.h>

/* --------------------------------------------------------------------------
 * Key Input
 * -------------------------------------------------------------------------- */

typedef enum tui_key_type {
  TUI_KEY_NONE = 0,

  /* Control */
  TUI_KEY_ESC,
  TUI_KEY_ENTER,
  TUI_KEY_TAB,
  TUI_KEY_BACKSPACE,

  /* Movimiento */
  TUI_KEY_UP,
  TUI_KEY_DOWN,
  TUI_KEY_LEFT,
  TUI_KEY_RIGHT,
  TUI_KEY_HOME,
  TUI_KEY_END,
  TUI_KEY_PAGE_UP,
  TUI_KEY_PAGE_DOWN,
  TUI_KEY_INSERT,
  TUI_KEY_DELETE,

  /* Función */
  TUI_KEY_F1,
  TUI_KEY_F2,
  TUI_KEY_F3,
  TUI_KEY_F4,
  TUI_KEY_F5,
  TUI_KEY_F6,
  TUI_KEY_F7,
  TUI_KEY_F8,
  TUI_KEY_F9,
  TUI_KEY_F10,
  TUI_KEY_F11,
  TUI_KEY_F12,
  TUI_KEY_CHAR

} tui_key_type;

typedef enum key_mod {
  TUI_MOD_NONE  = 0,
  TUI_MOD_SHIFT = 1 << 0,
  TUI_MOD_CTRL  = 1 << 1,
  TUI_MOD_ALT   = 1 << 2
} tui_key_mod;

typedef struct tui_key {
  tui_key_type  type;
  tui_key_mod   mod;
  char          ch;           /* válido solo si key == TUI_KEY_CHAR */
} tui_key;

void tui_getkey(tui_key* key);

/* --------------------------------------------------------------------------
 * Line
 * -------------------------------------------------------------------------- */

typedef enum tui_align {
  TUI_CENTER_X = 1 << 0,
  TUI_CENTER_Y = 1 << 1,
  TUI_RIGHT    = 1 << 3,
  TUI_DOWN     = 1 << 5
} tui_align;

size_t tui_format_line(const char* raw, String* string);

/* --------------------------------------------------------------------------
 * File
 * -------------------------------------------------------------------------- */

typedef struct tui_file {
  int     allocated;
  int     off_x;
  int     off_y;
  int     pos_x;
  int     pos_y;
  int     write;
  VECstr  text;
} tui_file;

tui_file* tui_file_create(tui_file* file, const char* raw, int write);
void      tui_file_destroy(tui_file* file);

/* --------------------------------------------------------------------------
 * Box
 * -------------------------------------------------------------------------- */

typedef struct tui_box {
  int allocated;
  int x, y, w, h;       /* Coord x and y, width and height  */
  int up, rt, dn, lt;   /* Padding up, right, down and left */  
  const char* bg_1;     /* For unselected background */
  const char* bg_2;     /* For selected background */
  const char* fg_1;     /* For unselected foreground */
  const char* fg_2;     /* For selected foreground */
} tui_box;

tui_box*  tui_box_create(tui_box* box, int x, int y, int w, int h);
void      tui_box_destroy(tui_box* box);

void      tui_box_pad(tui_box* box, int up, int right, int down, int left);
void      tui_box_style(tui_box* box, const char* bg_1, const char* bg_2, const char* fg_1, const char* fg_2);

void      tui_box_clear(tui_box* box);
void      tui_box_write_line(tui_box* box, String* line, tui_align align);
void      tui_box_write_file(tui_box* box, tui_file* file);

/* --------------------------------------------------------------------------
 * Window
 * -------------------------------------------------------------------------- */

typedef struct tui_window{
  int hide_title;
  tui_box title;
  tui_box content;
} tui_window;

/* --------------------------------------------------------------------------
 * TUI State
 * -------------------------------------------------------------------------- */

typedef struct tui_console_status {
  #ifdef OS_WINDOWS
    UINT in_cp;
    UINT out_cp;
    DWORD in_mode;
    DWORD out_mode;
    HANDLE in_handle;
    HANDLE out_handle;
  #else

  #endif
} tui_console_status;

typedef struct tui_screen_t {
  tui_box header;
  tui_box footer;

  int w, h;

  tui_console_status cns;
  tui_console_status cns_old;
} tui_screen_t;

extern tui_screen_t tui_screen;

void tui_init(void);
void tui_cleanup(void);

void tui_print(void);

#endif /* __VERBA_TUI_H__ */
