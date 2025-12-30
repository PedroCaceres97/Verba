#include <verba/log.h>
#include <verba/blop.h>

static FILE* debug = NULL;
static FILE* error = NULL;

void log_init() {
  debug = fopen("debug.txt", "wb");
  error = fopen("errors.txt", "wb");
}
void log_cleanup() {
  LOG_DEBUG("Log Cleanup");
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