#ifndef __BLOP_FILE_H__
#define __BLOP_FILE_H__

#include <blop.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t  blop_file_size(FILE* f);
char*   blop_file_dump(FILE* f);

int     blop_dir_make(const char* dir);
int     blop_dir_exist(const char* dir);
int     blop_file_exist(const char* file);

#ifdef BLOP_FILE_IMPLEMENTATION

size_t  blop_file_size(FILE* f) {
  rewind(f);
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);
  return (size_t)size;
}
char*   blop_file_dump(FILE* f) {
  size_t size = blop_file_size(f);
  char* buffer = NULL;
  BLOP_CALLOC(buffer, char, size + 1);
  fread(buffer, 1, size, f);
  buffer[size] = '\0';
  return buffer;
}

int     blop_dir_make(const char* dir) {

}
int     blop_dir_exist(const char* dir) {

}
int     blop_file_exist(const char* file) {

}

#endif

#ifdef __cplusplus
}
#endif

#endif /* __BLOP_FILE_H__ */
