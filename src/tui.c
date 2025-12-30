#include <verba/tui.h>

/* --------------------------------------------------------------------------
 * TUI State
 * -------------------------------------------------------------------------- */

static tui_screen screen = {0};

void tui_init() {
  #ifdef OS_WINDOWS
    screen.cns_old.in_cp = GetConsoleCP();
    screen.cns_old.out_cp = GetConsoleOutputCP();
    screen.cns_old.in_handle = GetStdHandle(STD_INPUT_HANDLE);
    screen.cns_old.out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT_FORCED(screen.cns_old.in_handle != INVALID_HANDLE_VALUE, "Function failed -> GetStdHandle(STD_INPUT_HANDLE)");
    ASSERT_FORCED(screen.cns_old.out_handle != INVALID_HANDLE_VALUE, "Function failed -> GetStdHandle(STD_OUTPUT_HANDLE)");
    ASSERT_FORCED(GetConsoleMode(screen.cns_old.in_handle, &screen.cns_old.in_mode) != 0, "Function failed -> GetConsoleMode(screen.cns_old.in_handle, &screen.cns_old.in_mode)");
    ASSERT_FORCED(GetConsoleMode(screen.cns_old.out_handle, &screen.cns_old.out_mode) != 0, "Function failed -> GetConsoleMode(screen.cns_old.out_handle, &screen.cns_old.out_mode)");

    screen.cns.in_cp = CP_UTF8;
    screen.cns.out_cp = CP_UTF8;
    screen.cns.in_handle = screen.cns_old.in_handle;
    screen.cns.out_handle = screen.cns_old.out_handle;
    screen.cns.out_mode = (screen.cns_old.out_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)) | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT;
    
    SetConsoleCP(screen.cns.in_cp);
    SetConsoleOutputCP(screen.cns.out_cp);
    ASSERT_FORCED(SetConsoleMode(screen.cns.out_handle, screen.cns.out_mode) != 0, "Function failed -> SetConsoleMode(screen.cns.out_handle, screen.cns.out_mode)");
    LOG_DEBUG("Function success -> SetConsoleMode(screen.cns.out_handle, screen.cns.out_mode)");
    
    printf(ANSI_CLEAR_SCREEN ANSI_CURSOR_HIDE);

    CONSOLE_SCREEN_BUFFER_INFO info;
    ASSERT_FORCED(GetConsoleScreenBufferInfo(screen.cns.out_handle, &info), "Function failed ->GetConsoleScreenBufferInfo(screen.cns.out_handle, &info)");
    LOG_DEBUG("Function success -> GetConsoleScreenBufferInfo(screen.cns.out_handle, &info)");
    
    screen.w = info.srWindow.Right - info.srWindow.Left + 1;
    screen.h = info.srWindow.Bottom - info.srWindow.Top + 1;
  #else

  #endif

  tui_box_create(&screen.header, 0, 0, screen.w, 1, TUI_LINE);
  tui_box_style(&screen.header, ANSI_BG_RGB(39, 247, 27), ANSI_FG_BLACK, ANSI_BG_RGB(39, 247, 27), ANSI_FG_BLACK);
  tui_box_text(&screen.header, "Bienvenido a Verba - Developer Version", TUI_TEXT_CENTER_X | TUI_TEXT_CENTER_Y);
  tui_box_pad(&screen.header, 0, 0, 0, 0);

  tui_box_create(&screen.footer, 0, screen.h, screen.w, 1, TUI_LINE);
  tui_box_style(&screen.footer, ANSI_BG_RGB(39, 247, 27), ANSI_FG_BLACK, ANSI_BG_RGB(39, 247, 27), ANSI_FG_BLACK);
  tui_box_text(&screen.footer, "(ESC = Salir)", TUI_TEXT_CENTER_X | TUI_TEXT_CENTER_Y);
  tui_box_pad(&screen.footer, 0, 0, 0, 0);

  LOG_DEBUG("Variables initialized, drawing...");

  tui_draw(&screen.header);
  tui_draw(&screen.footer);
}
void tui_cleanup() {
  LOG_DEBUG("TUI Cleanup");

  printf(ANSI_CLEAR_SCREEN ANSI_CURSOR_HOME ANSI_CURSOR_SHOW);

  #ifdef OS_WINDOWS
    SetConsoleCP(screen.cns_old.in_cp);
    SetConsoleOutputCP(screen.cns_old.out_cp);
    ASSERT_FORCED(SetConsoleMode(screen.cns_old.out_handle, screen.cns_old.out_mode) != 0, "Function failed -> SetConsoleMode(screen.cns_old.out_handle, screen.cns_old.out_mode)");
  #else

  #endif
}

int tui_screenw() {
  return screen.w;
}
int tui_screenh() {
  return screen.h;
}

/* --------------------------------------------------------------------------
 * Key Input
 * -------------------------------------------------------------------------- */

void tui_getkey(tui_key* key) {
  #ifdef OS_WINDOWS
    INPUT_RECORD rec;
    DWORD count = 0;

    key->type = TUI_KEY_NONE;
    key->mod  = TUI_MOD_NONE;
    key->ch   = 0;

    if (!PeekConsoleInput(screen.cns.in_handle, &rec, 1, &count) || count == 0) { return; }

    ReadConsoleInput(screen.cns.in_handle, &rec, 1, &count);

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

static String* tui_format_line(const char* text, size_t* parsed) {
  const char* nl = strchr(text, '\n');
  size_t size = nl ? (size_t)(nl - text) : strlen(text);

  String* line = {0};
  string_create(&line);
  string_resize(&line, size);
  string_strcpy(&line, 0, text, size);

  *parsed = size + (nl != NULL);

  return line;
}

void tui_format_text(const char* text) {
  VECstr lines = vecstr_create(NULL);

  size_t len = strlen(text);
  size_t parsed = 0;

  while(true) {
    string str = tui_format_line(text);
    tui_lines_push_back(lines, str);
    parsed = str.size;
    if (parsed == len) {
      break;
    } else {
      len -= parsed;
      text += parsed;
    }
  };

  return lines;
};

/* --------------------------------------------------------------------------
 * Box
 * -------------------------------------------------------------------------- */

tui_box*  tui_box_create(tui_box* box, int x, int y, int w, int h, tui_box_type type) {
  if (!box) {
    box = verba_calloc(tui_box, 1, "tui_box");
    box->allocated = true;
  } else {
    box->allocated = false;
  }

  box->x = x;
  box->y = y;
  box->w = w;
  box->h = h;
  box->type = type;

  if (type == TUI_LINE) {
    string_create(&box->data.line);
  } else if (type == TUI_PAGE) {
    tui_lines_create(box->data.page.text);
  }

  return box;
}
void      tui_box_destroy(tui_box* box) {
  tui_lines_clear(box->text);
  tui_lines_destroy(box->text);

  if (box->allocated) {
    verba_free(box);
  }
}

void      tui_box_pad(tui_box* box, int up, int right, int down, int left) {
  box->up = up;
  box->rt = right;
  box->dn = down;
  box->lt = left;
}
void      tui_box_text(tui_box* box, const char* text, tui_text_align align) {
  box->text = tui_format_text(text);
  box->align = align;
}
void      tui_box_style(tui_box* box, const char* bg, const char* fg, const char* bg_s, const char* fg_s) {
  box->style.bg   = bg;
  box->style.fg   = fg;
  box->style.bg_s = bg_s;
  box->style.fg_s = fg_s;
}

static void tui_fill(size_t count, int x, int y, const char* bg);
static void tui_puts(const char* text, size_t count, int x, int y, const char* bg, const char* fg);

static void tui_draw_line(tui_box* box);
static void tui_draw_page(tui_box* box);

void tui_draw(tui_box* box) {
  char pos[64] = {0};

  LOG_DEBUG("Drawing...");
  for (int y = 0; y < box->h; y++) {
    tui_fill(box->w, box->x, box->y + y, box->style.bg);
  }

  switch(box->type) {
    case TUI_LINE: {
      tui_draw_line(box);
      break;
    }
    case TUI_PAGE: {
      tui_draw_page(box);
      break;
    }
  }
}

static void tui_draw_line(tui_box* box) {
  int pos_x = box->x + box->lt;
  int pos_y = box->y + box->up;
  int max_x = box->w - box->lt - box->rt;
  int max_y = box->h - box->up - box->dn;
  int align_x = 0;
  int align_y = 0;

  string line = tui_lines_get(box->text, 0);
  size_t len = line.size;
  const char* cstr = line.data;

  LOG_DEBUG("Drawing line...");

  if (box->align & TUI_TEXT_CENTER_Y) {
    align_y = max_y / 2 + ((max_x % 2) == 0);
  } else if (box->align & TUI_TEXT_DOWN) {
    align_y = max_y - 1;
  }
  pos_y += align_y;

  if (len > max_x) {
    tui_puts(cstr, max_x - 3, pos_x, pos_y, box->style.bg, box->style.fg);
    tui_puts("...", 3, pos_x + max_x - 3, pos_y, box->style.bg, box->style.fg);
    return;
  }

  if (box->align & TUI_TEXT_CENTER_X) {
    align_x = (max_x - len) / 2 + (((max_x - len) % 2) == 0);
  } else if (box->align & TUI_TEXT_RIGHT) {
    align_x = max_x - len;
  }
  pos_x += align_x;

  tui_puts(cstr, len, pos_x, pos_y, box->style.bg, box->style.fg);
  return;
}
static void tui_draw_page(tui_box* box) {
  int pos_x = box->x + box->lt;
  int pos_y = box->y + box->up;
  int max_x = box->w - box->lt - box->rt;
  int max_y = box->h - box->up - box->dn;
  int align_x = 0;
  int align_y = 0;

  LOG_DEBUG("Drawing page...");

  for (int i = 0; i < box->text->size && i < max_y; i++) {
    string line = tui_lines_get(box->text, i); 
    size_t len = line.size;
    const char* cstr = line.data;
    tui_puts(cstr, MIN(len, max_x), pos_x, pos_y + i, box->style.bg, box->style.fg);
  }
}

static void tui_fill(size_t count, int x, int y, const char* bg) {
  char pos[64] = {0};

  /*
    1. %s     Sets cursor position
    2. %s     Sets background color
    4. %*.0s  Writes exactly count bytes of empty space
    5. %s     Resets color
  */
  printf("%s%s%*.0s%s",
          ansi_cursor_pos(pos, x, y),
          bg, count, "",
          ANSI_RESET);
}
static void tui_puts(const char* text, size_t count, int x, int y, const char* bg, const char* fg) {
  char pos[64] = {0};

  /*
    1. %s     Sets cursor position
    2. %s     Sets background color
    3. %s     Sets foreground color
    4. %.*s   Writes exactly count bytes of text
    5. %s     Resets color
  */
  printf("%s%s%s%.*s%s",
          ansi_cursor_pos(pos, x, y),
          bg, fg,
          count, text,
          ANSI_RESET);
}