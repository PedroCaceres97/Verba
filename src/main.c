#include <verba/mem.h>
#include <verba/tui.h>
#include <verba/ai.h>

size_t file_size(FILE* file) {
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  ASSERT(size >= 0, "Failed function -> ftell(file)");
  rewind(file);
  return (size_t)size;
}
char*  file_dump(FILE* file) {
  size_t size = file_size(file);
  char* dump = mem_malloc(char, size + 1, "File Dump");
  fread(dump, 1, size, file);
  dump[size] = '\0';
  return dump;
}
char*  file_open_dump(const char* path) {
  FILE* file = fopen(path, "rb");
  ASSERT(file != NULL, "Failed to open file");
  char* dump = file_dump(file);
  ASSERT(fclose(file) == 0, "Failed to close file");
  return dump;
}

int main() {
  log_init();
  mem_init();
  tui_init();
  ai_init();

  if (tui_screenw() < 80 || tui_screenh() < 20) {
    ai_cleanup();
    tui_cleanup();
    log_cleanup();

    printf("[VERBA]:\n Context: %s:%u (%s)\n Message: Para jugar verba se necesita una consola con un minimo de 80 de ancho y 20 de alto. Su consola mide %i de ancho y %i de alto\n\n", FILE_PATH, __LINE__, __func__, tui_screenw(), tui_screenh());
    return -1;
  }

  char* lorem = file_open_dump("C:/Dev/Verba/main.c");

  int w = tui_screenw();
  int h = tui_screenh();
  tui_box menu = {0};
  tui_box_create(&menu, 0, 0, 0, 0, TUI_PAGE);
  tui_box_style(&menu, ANSI_BG_CYAN, ANSI_FG_BLACK, ANSI_BG_CYAN, ANSI_FG_BLACK);
  tui_box_text(&menu, lorem, TUI_TEXT_LEFT);
  tui_box_pad(&menu, 1, 2, 1, 2);
  menu.w = tui_screenw() - 10;
  menu.h = tui_screenh() - 4;
  menu.x = (tui_screenw() - menu.w) / 2 + 1;
  menu.y = (tui_screenh() - menu.h) / 2 + 1;

  tui_draw(&menu);

  tui_key key;
  while (true) {
    tui_getkey(&key);
    if (key.type == TUI_KEY_ESC) {
      break;
    }
  }

  mem_free(lorem);

  ai_cleanup();
  tui_cleanup();
  track_init();
  log_cleanup();
  return 0;
}