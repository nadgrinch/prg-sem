/*
  filename: keyboard.c
  author: dratvsim
*/

#include <stdio.h>
#include <stdlib.h>

#include "keyboard.h"

#include "event_queue.h"
#include "utils.h"

#ifndef TIMEOUT
#define TIMEOUT 100
#endif

void* keyboard_thread(void* d){
  info("keyboard_thread - start");
  call_termios(0);
  unsigned char c;
  event ev;
  bool quit = false;
  while (!quit){
    ev.type = EV_TYPE_NUM;
    // non-blicking getchar()
    int ret = my_getchar_timeout(TIMEOUT, &c);
    if (ret != 0) {
      switch(c) {
        case 'q':
          // quit program
          ev.type = EV_QUIT;
          break;
        case 'g':
          // get version
          ev.type = EV_GET_VERSION;
          break;
        case 'a':
          // abort
          ev.type = EV_ABORT;
          break;
        case 's':
          // set compute
          ev.type = EV_SET_COMPUTE;
          break;
        case 'c':
          // compute
          ev.type = EV_COMPUTE_CPU;
          break;
        case '1':
          // compute
          ev.type = EV_COMPUTE;
          break;
        case 'r':
          // reset cid
          ev.type = EV_RESET_CHUNK;
          break;
        case 'l':
          // clear buffer
          ev.type = EV_CLEAR_BUFFER;
          break;
        case 'p':
          // refresh window with buffer
          ev.type = EV_REFRESH;
          break;
        case 'h':
          // help msg
          ev.type = EV_HELP;
          break;
        case 'x':
          // change param
          ev.type = EV_CHANGE_PARAM;
          break;
        case 'm':
          // toggle animation
          ev.type = EV_ANIMATE;
          break;
        case 'z':
          ev.type = EV_ZOOM;
          break;
        case 'j':
          // save menu open
          ev.type = EV_SAVE_IMG;
          break;
        default:
          info("Wrong Keyboard input, press 'h' for help");
          break;
      }
    }
    if (ev.type != EV_TYPE_NUM) {
      queue_push(ev);
    }
    quit = is_quit();
  }
  call_termios(1);
  info("keyboard_thread - finish");
  return NULL;
}
