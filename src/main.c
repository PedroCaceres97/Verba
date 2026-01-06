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
  char* dump = mem_malloc(char, size + 1);
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

void init() {
  log_init();
  mem_init();
  tui_init();

  if (tui_screen.w < 80 || tui_screen.h < 20) {
    tui_cleanup();
    mem_cleanup();
    log_cleanup();

    printf("[VERBA]:\n Context: %s:%u (%s)\n Message: Para jugar verba se necesita una consola con un minimo de 80 de ancho y 20 de alto. Su consola mide %i de ancho y %i de alto\n\n", FILE_PATH, __LINE__, __func__, tui_screen.w, tui_screen.h);
    ABORT();
  }

  ai_init();
}
void cleanup() {
  ai_cleanup();
  tui_cleanup();
  mem_cleanup();
  log_cleanup();
}

int main() {
  init();

  ai_message("Write a litle story about a fantasy world");

  tui_box menu = {0};
  tui_box_create(&menu, 0, 0, 0, 0);
  tui_box_style(&menu, ANSI_BG_CYAN, ANSI_BG_CYAN, ANSI_FG_BLACK, ANSI_FG_BLACK);
  tui_box_pad(&menu, 1, 2, 1, 2);
  menu.w = tui_screen.w - 10;
  menu.h = tui_screen.h - 4;
  menu.x = (tui_screen.w - menu.w) / 2 + 1;
  menu.y = (tui_screen.h - menu.h) / 2 + 1;

  tui_file file;
  tui_file_create(&file, ai_last(), false);
  tui_box_write_file(&menu, &file);
  tui_print();

  int running = true;
  tui_key key;
  while (running) {
    tui_getkey(&key);
    switch (key.type)
    {
    case TUI_KEY_NONE:
      continue;
      break;
    case TUI_KEY_ESC:
      running = false;
      break;
    case TUI_KEY_UP:
      if (file.off_y != 0) {
        file.off_y--;
        tui_box_write_file(&menu, &file);
      }
      break;
    case TUI_KEY_DOWN:
      if (file.off_y != (int)file.text.size - (menu.h - (menu.up + menu.dn))) {
        file.off_y++;
        tui_box_write_file(&menu, &file);
      }
      break;
    default:
      break;
    }
    tui_print();
  }

  tui_file_destroy(&file);

  cleanup();
  return 0;
}