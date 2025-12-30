#ifndef __VERBA_LOG_H__
#define __VERBA_LOG_H__

void log_init();
void log_cleanup();

void log_stdout(const char* format, ...);
void log_stderr(const char* format, ...);

#endif /* __VERBA_LOG_H__ */