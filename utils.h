/*
  filename: utils.h
  author: dratvsim
*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
#include <stdlib.h>


void my_assert(bool r, const char *fcname, int line, const char *fname);
void* my_alloc(size_t size);
int my_getchar_timeout(int timeout_ms, unsigned char *c);
void call_termios(int reset);

void info(const char *str);
void debug(const char *str);
void warn(const char *str);
void error(const char *str);

#endif

/*end of utils.h*/