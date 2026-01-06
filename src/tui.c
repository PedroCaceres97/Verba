#include <verba/tui.h>

/* --------------------------------------------------------------------------
 * Helpers
 * -------------------------------------------------------------------------- */

static int changed = false;
static char tui_buffer[64 * 1024] = {0};
static char* tui_cursor = tui_buffer;
static const char* tui_last_bg = NULL;
static const char* tui_last_fg = NULL;

static inline void tui_emit(const char* s, size_t n) {
  memcpy(tui_cursor, s, n);
  tui_cursor += n;
  changed = true;
}
static inline void tui_emit_bg(const char* bg) {
  if (tui_last_bg != bg) {
    tui_last_bg = bg;
    size_t n = strlen(bg);
    memcpy(tui_cursor, bg, n);
    tui_cursor += n;
    changed = true;
  }
}
static inline void tui_emit_fg(const char* fg) {
  if (tui_last_fg != fg) {
    tui_last_fg = fg;
    size_t n = strlen(fg);
    memcpy(tui_cursor, fg, n);
    tui_cursor += n;
    changed = true;
  }
}
static inline void tui_emit_pos(int x, int y) {
  int i = 0;
  char temp[16] = {0};
  
  *tui_cursor++ = '\x1b';
  *tui_cursor++ = '[';

  do {
    temp[i++] = '0' + (y % 10);
    y /= 10;
  } while(y != 0);
  while(i > 0) {
    *tui_cursor++ = temp[--i];
  }

  *tui_cursor++ = ';';

  do {
    temp[i++] = '0' + (x % 10);
    x /= 10;
  } while(x != 0);
  while(i > 0) {
    *tui_cursor++ = temp[--i];
  }
  
  *tui_cursor++ = 'H';
  changed = true;
}
static inline void tui_emit_spaces(size_t count) {
  memset(tui_cursor, ' ', count);
  tui_cursor += count;
  changed = true;
}

static void tui_fill(int x, int y, const char* bg, size_t count) {
  tui_emit_pos(x, y);
  tui_emit_bg(bg);
  tui_emit_spaces(count);
}
static void tui_puts(int x, int y, const char* bg, const char* fg, const char* text, size_t count) {
  tui_emit_pos(x, y);
  tui_emit_bg(bg);
  tui_emit_fg(fg);
  tui_emit(text, count);
}
void tui_print(void) {
  if (!changed) { return; }

  fwrite(tui_buffer, 1, tui_cursor - tui_buffer, stdout);
  memset(tui_buffer, 0, sizeof(tui_buffer));
  tui_cursor = tui_buffer;
  tui_last_bg = NULL;
  tui_last_fg = NULL;
  changed = false;
}

/* --------------------------------------------------------------------------
 * Key Input
 * -------------------------------------------------------------------------- */

void      tui_getkey(tui_key* key) {
  #ifdef OS_WINDOWS
    INPUT_RECORD rec;
    DWORD count = 0;

    key->type = TUI_KEY_NONE;
    key->mod  = TUI_MOD_NONE;
    key->ch   = 0;

    if (!PeekConsoleInput(tui_screen.cns.in_handle, &rec, 1, &count) || count == 0) { return; }

    ReadConsoleInput(tui_screen.cns.in_handle, &rec, 1, &count);

    if (rec.EventType != KEY_EVENT) { return; }

    KEY_EVENT_RECORD* k = &rec.Event.KeyEvent;
    if (!k->bKeyDown) { return; }

    if (k->dwControlKeyState & SHIFT_PRESSED)                             { key->mod |= TUI_MOD_SHIFT; }
    if (k->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))  { key->mod |= TUI_MOD_CTRL; }
    if (k->dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))    { key->mod |= TUI_MOD_ALT; }
  
    if (k->uChar.AsciiChar >= 32 && k->uChar.AsciiChar <= 126) {
      key->type = TUI_KEY_CHAR;
      key->ch  = k->uChar.AsciiChar;
      return;
    }

    switch (k->wVirtualKeyCode) {
      case VK_ESCAPE:   key->type = TUI_KEY_ESC; break;
      case VK_RETURN:   key->type = TUI_KEY_ENTER; break;
      case VK_TAB:      key->type = TUI_KEY_TAB; break;
      case VK_BACK:     key->type = TUI_KEY_BACKSPACE; break;

      case VK_UP:       key->type = TUI_KEY_UP; break;
      case VK_DOWN:     key->type = TUI_KEY_DOWN; break;
      case VK_LEFT:     key->type = TUI_KEY_LEFT; break;
      case VK_RIGHT:    key->type = TUI_KEY_RIGHT; break;

      case VK_HOME:     key->type = TUI_KEY_HOME; break;
      case VK_END:      key->type = TUI_KEY_END; break;
      case VK_PRIOR:    key->type = TUI_KEY_PAGE_UP; break;
      case VK_NEXT:     key->type = TUI_KEY_PAGE_DOWN; break;

      case VK_INSERT:   key->type = TUI_KEY_INSERT; break;
      case VK_DELETE:   key->type = TUI_KEY_DELETE; break;

      case VK_F1:       key->type = TUI_KEY_F1; break;
      case VK_F2:       key->type = TUI_KEY_F2; break;
      case VK_F3:       key->type = TUI_KEY_F3; break;
      case VK_F4:       key->type = TUI_KEY_F4; break;
      case VK_F5:       key->type = TUI_KEY_F5; break;
      case VK_F6:       key->type = TUI_KEY_F6; break;
      case VK_F7:       key->type = TUI_KEY_F7; break;
      case VK_F8:       key->type = TUI_KEY_F8; break;
      case VK_F9:       key->type = TUI_KEY_F9; break;
      case VK_F10:      key->type = TUI_KEY_F10; break;
      case VK_F11:      key->type = TUI_KEY_F11; break;
      case VK_F12:      key->type = TUI_KEY_F12; break;

      default: return;
  }
  #else

  #endif
}

/* --------------------------------------------------------------------------
 * Text
 * -------------------------------------------------------------------------- */

size_t    tui_format_line(const char* raw, String* line) {
  const char* nl = strchr(raw, '\n');
  size_t size = TERNARY(nl != NULL, (size_t)(nl - raw), strlen(raw));

  string_resize(line, size);
  string_strcpy(line, 0, raw, size);

  return size + (nl != NULL);
}

/* --------------------------------------------------------------------------
 * File
 * -------------------------------------------------------------------------- */

tui_file* tui_file_create(tui_file* file, const char* raw, int write) {
  if (!file) {
    file = mem_calloc(struct tui_file, 1);
    file->allocated = true;
  } else {
    file->allocated = false;
  }
  file->off_x = 0;
  file->off_y = 0;
  file->pos_x = 0;
  file->pos_y = 0;
  file->write = write;
  vecstr_create(&file->text);

  size_t len = strlen(raw);
  while(true) {
    String* line = mem_string_alloc();
    size_t parsed = tui_format_line(raw, line);
    vecstr_push_back(&file->text, line);
    if (parsed == len) {
      break;
    } else {
      raw += parsed;
      len -= parsed;
    }
  };
  return file;
}
void      tui_file_destroy(tui_file* file) {
  vecstr_clear(&file->text);
  vecstr_destroy(&file->text);

  if (file->allocated) {
    mem_free(file);
  }
}

/* --------------------------------------------------------------------------
 * Box
 * -------------------------------------------------------------------------- */

tui_box*  tui_box_create(tui_box* box, int x, int y, int w, int h) {
  if (!box) {
    box = mem_calloc(struct tui_box, 1);
    box->allocated = true;
  } else {
    box->allocated = false;
  }

  box->x = x;
  box->y = y;
  box->w = w;
  box->h = h;
  box->up = 0;
  box->lt = 0;
  box->dn = 0;
  box->rt = 0;
  box->bg_1 = ANSI_BG_WHITE;
  box->bg_2 = ANSI_BG_BLACK;
  box->fg_1 = ANSI_FG_BLACK;
  box->fg_2 = ANSI_FG_WHITE;

  return box;
}
void      tui_box_destroy(tui_box* box) {
  if (box->allocated) {
    mem_free(box);
  }
}

void      tui_box_pad(tui_box* box, int up, int right, int down, int left) {
  box->up = up;
  box->rt = right;
  box->dn = down;
  box->lt = left;
}
void      tui_box_style(tui_box* box, const char* bg_1, const char* bg_2, const char* fg_1, const char* fg_2) {
  box->bg_1 = bg_1;
  box->bg_2 = bg_2;
  box->fg_1 = fg_1;
  box->fg_2 = fg_2;
}

void      tui_box_clear(tui_box* box) {
  for (int y = 0; y < box->h; y++) {
    tui_fill(box->x, box->y + y, box->bg_1, box->w);
  }
}
void      tui_box_write_line(tui_box* box, String* line, tui_align align) {
  tui_box_clear(box);

  int pos_x = box->x + box->lt;
  int pos_y = box->y + box->up;
  int max_x = box->w - box->lt - box->rt;
  int max_y = box->h - box->up - box->dn;
  int align_x = 0;
  int align_y = 0;

  int len = (int)line->size;
  const char* cstr = line->data;

  if (align & TUI_CENTER_Y) {
    align_y = max_y / 2 + ((max_y % 2) == 0);
  } else if (align & TUI_DOWN) {
    align_y = max_y - 1;
  }
  pos_y += align_y;

  if (len > max_x) {
    tui_puts(pos_x, pos_y, box->bg_1, box->fg_1, cstr, max_x - 3);
    tui_puts(pos_x + max_x - 3, pos_y, box->bg_1, box->fg_1, "...", 3);
    return;
  }

  if (align & TUI_CENTER_X) {
    align_x = (max_x - len) / 2 + (((max_x - len) % 2) == 0);
  } else if (align & TUI_RIGHT) {
    align_x = max_x - len;
  }
  pos_x += align_x;

  tui_puts(pos_x, pos_y, box->bg_1, box->fg_1, cstr, len);
  return;
}
void      tui_box_write_file(tui_box* box, tui_file* file) {
  tui_box_clear(box);

  int pos_x = box->x + box->lt;
  int pos_y = box->y + box->up;
  int max_x = box->w - box->lt - box->rt;
  int max_y = box->h - box->up - box->dn;

  for (int y = 0; y < max_y; y++) {
    String* line = vecstr_get(&file->text, y + file->off_y);
    int len = MIN((int)line->size, max_x);

    if (len > 0) {
      tui_puts(pos_x, pos_y + y, box->bg_1, box->fg_1, line->data, len);
    }

    if (len < max_x) {
      tui_fill(pos_x + len, pos_y + y, box->bg_1, max_x - len);
    }
  }
}

/* --------------------------------------------------------------------------
 * TUI State
 * -------------------------------------------------------------------------- */

tui_screen_t tui_screen = {0};
String* header = NULL;
String* footer = NULL;

void tui_init(void) {
  #ifdef OS_WINDOWS
    tui_screen.cns_old.in_cp = GetConsoleCP();
    tui_screen.cns_old.out_cp = GetConsoleOutputCP();
    tui_screen.cns_old.in_handle = GetStdHandle(STD_INPUT_HANDLE);
    tui_screen.cns_old.out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT_FORCED(tui_screen.cns_old.in_handle != INVALID_HANDLE_VALUE, "Function failed -> GetStdHandle(STD_INPUT_HANDLE)");
    ASSERT_FORCED(tui_screen.cns_old.out_handle != INVALID_HANDLE_VALUE, "Function failed -> GetStdHandle(STD_OUTPUT_HANDLE)");
    ASSERT_FORCED(GetConsoleMode(tui_screen.cns_old.in_handle, &tui_screen.cns_old.in_mode) != 0, "Function failed -> GetConsoleMode(tui_screen.cns_old.in_handle, &tui_screen.cns_old.in_mode)");
    ASSERT_FORCED(GetConsoleMode(tui_screen.cns_old.out_handle, &tui_screen.cns_old.out_mode) != 0, "Function failed -> GetConsoleMode(tui_screen.cns_old.out_handle, &tui_screen.cns_old.out_mode)");

    tui_screen.cns.in_cp = CP_UTF8;
    tui_screen.cns.out_cp = CP_UTF8;
    tui_screen.cns.in_handle = tui_screen.cns_old.in_handle;
    tui_screen.cns.out_handle = tui_screen.cns_old.out_handle;
    tui_screen.cns.out_mode = tui_screen.cns_old.out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT;
    
    SetConsoleCP(tui_screen.cns.in_cp);
    SetConsoleOutputCP(tui_screen.cns.out_cp);
    ASSERT_FORCED(SetConsoleMode(tui_screen.cns.out_handle, tui_screen.cns.out_mode) != 0, "Function failed -> SetConsoleMode(tui_screen.cns.out_handle, tui_screen.cns.out_mode)");
    
    printf(ANSI_CLEAR_SCREEN ANSI_CURSOR_HIDE);

    CONSOLE_SCREEN_BUFFER_INFO info;
    ASSERT_FORCED(GetConsoleScreenBufferInfo(tui_screen.cns.out_handle, &info), "Function failed ->GetConsoletui_screenBufferInfo(tui_screen.cns.out_handle, &info)");
    
    tui_screen.w = info.srWindow.Right - info.srWindow.Left + 1;
    tui_screen.h = info.srWindow.Bottom - info.srWindow.Top + 1;
  #else

  #endif

  tui_box_create(&tui_screen.header, 0, 0, tui_screen.w, 1);
  tui_box_style(&tui_screen.header, ANSI_BG_RGB(39, 247, 27), ANSI_BG_RGB(39, 247, 27), ANSI_FG_BLACK, ANSI_FG_BLACK);

  tui_box_create(&tui_screen.footer, 0, tui_screen.h, tui_screen.w, 1);
  tui_box_style(&tui_screen.footer, ANSI_BG_RGB(39, 247, 27), ANSI_BG_RGB(39, 247, 27), ANSI_FG_BLACK, ANSI_FG_BLACK);

  header = mem_string_alloc();
  footer = mem_string_alloc();

  tui_format_line("Bienvenido a Verba - Developer Version", header);
  tui_format_line("(ESC = Salir)", footer);

  tui_box_write_line(&tui_screen.header, header, TUI_CENTER_X);
  tui_box_write_line(&tui_screen.footer, footer, TUI_CENTER_X);

  LOG_SUCCESS("Tui initialized");
}
void tui_cleanup(void) {
  printf(ANSI_RESET ANSI_CLEAR_SCREEN ANSI_CURSOR_HOME ANSI_CURSOR_SHOW);

  mem_string_free(header);
  mem_string_free(footer);

  #ifdef OS_WINDOWS
    SetConsoleCP(tui_screen.cns_old.in_cp);
    SetConsoleOutputCP(tui_screen.cns_old.out_cp);
    ASSERT_FORCED(SetConsoleMode(tui_screen.cns_old.out_handle, tui_screen.cns_old.out_mode) != 0, "Function failed -> SetConsoleMode(tui_screen.cns_old.out_handle, tui_screen.cns_old.out_mode)");
  #else

  #endif

  LOG_SUCCESS("Tui terminated");
}