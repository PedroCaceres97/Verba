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
 * Box
 * -------------------------------------------------------------------------- */

typedef struct tui_style {
  const char* fg;
  const char* bg;
  const char* bg_s; /* Background when item is selected */
  const char* fg_s; /* Foreground when item is selected */
} tui_style;

typedef enum tui_box_type {
  TUI_LINE,
  TUI_PAGE,
  TUI_FILE,
  TUI_CONFIRM,
  TUI_YES_NO,
  TUI_MENU,
  TUI_INPUT,
  TUI_PARENT
} tui_box_type;

typedef enum tui_text_align {
  TUI_TEXT_NONE     = 0,
  TUI_TEXT_CENTER_X = 1 << 0,
  TUI_TEXT_CENTER_Y = 1 << 1,
  TUI_TEXT_LEFT     = 1 << 2,
  TUI_TEXT_RIGHT    = 1 << 3,
  TUI_TEXT_UP       = 1 << 4,
  TUI_TEXT_DOWN     = 1 << 5
} tui_text_align;

typedef struct tui_page {
  int     off_x;
  int     off_y;
  VECstr  text;
} tui_page;

typedef struct tui_file {
  int     off_x;
  int     off_y;
  int     pos_x;
  int     pos_y;
  VECstr  text;
} tui_file;

typedef struct tui_box {
  int allocated;
  int x, y, w, h;       /* Coord x and y, width and height  */
  int up, rt, dn, lt;   /* Padding up, right, down and left */  
  tui_style style;
  tui_box_type type;
  
  union {
    String    line;
    tui_page  page;
    tui_file  file;
  } data;

  void (*key)(tui_key);
} tui_box;

tui_box*  tui_box_create(tui_box* box, int x, int y, int w, int h, tui_box_type type);
void      tui_box_destroy(tui_box* box);

void      tui_box_pad(tui_box* box, int up, int right, int down, int left);
void      tui_box_text(tui_box* box, const char* text, tui_text_align align);
void      tui_box_style(tui_box* box, const char* bg, const char* fg, const char* bg_s, const char* fg_s);

void      tui_draw(tui_box* box);

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

typedef struct tui_screen {
  tui_box header;
  tui_box footer;

  int w, h;

  tui_console_status cns;
  tui_console_status cns_old;
} tui_screen;

void tui_init();
void tui_cleanup();

int tui_screenw();
int tui_screenh();

#endif /* __VERBA_TUI_H__ */