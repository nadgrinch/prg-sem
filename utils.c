/*
  filename: utils.c
  author: dratvsim  
*/
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

#include "utils.h"

void my_assert(bool r, const char *fcname, int line, const char *fname){
  if (!r){
    fprintf(stderr, "ERROR: my_assert FAIL: %s() line %d in %s\n",fcname,line,fname);
    exit(101);
  }
}

void* my_alloc(size_t size){
  void *ret = malloc(size);
  if (!ret){
    fprintf(stderr, "ERROR: Cannot malloc!\n");
    exit(100);
  }

  return ret;
}

int my_getchar_timeout(int timeout_ms, unsigned char *c){
  struct pollfd poll_gc[1];
  int r = 0;
  poll_gc[0].fd = 0; // 0 ~ for stdin
  poll_gc[0].events = POLLIN | POLLRDNORM;

  if ((poll(&poll_gc[0], 1, timeout_ms) > 0) && (poll_gc[0].revents & (POLLIN | POLLRDNORM))) {
    r = read(0, c, 1);
  }
  return r;
}

void call_termios(int reset){
  static struct termios tio, tioOld;

  tcgetattr(STDIN_FILENO, &tio);
  if (reset){
    tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
  }
  else {
    tioOld = tio;
    cfmakeraw(&tio);
    tio.c_oflag |= OPOST;
    tcsetattr(STDIN_FILENO,TCSANOW, &tio);
  }
}

void info(const char *str){
  fprintf(stderr, "INFO: %s\n",str);
}

void debug(const char *str){
  fprintf(stderr, "DEBUG: %s\n",str);
}

void warn(const char *str){
  fprintf(stderr, "WARN: %s\n",str);
}

void error(const char *str){
  fprintf(stderr, "ERROR: %s\n",str);
}

/* end of utils.c*/