#include <verba/log.h>
#include <verba/blop.h>

static FILE* debug = NULL;
static FILE* error = NULL;

void log_init(void) {
  debug = fopen("debug.txt", "wb");
  error = fopen("errors.txt", "wb");
  if (debug == NULL) {
    fprintf(stderr, "%s\n", "Failed to open \"debug.txt\" in \"wb\"");
    ABORT();
  }
  if (error == NULL) {
    fprintf(stderr, "%s\n", "Failed to open \"debug.txt\" in \"wb\"");
    ABORT();
  }
  LOG("Log system initialized [FIRST LOG]");
  LOG_ERROR("Log system initialized [FIRST LOG]");
}
void log_cleanup(void) {
  LOG("Log system terminated [LAST LOG]");
  LOG_ERROR("Log system terminated [LAST LOG]");
  fclose(debug);
  fclose(error);
}

void log_stdout(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(debug, format, args);
  va_end(args);
}
void log_stderr(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(error, format, args);
  va_end(args);
}
