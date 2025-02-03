/*
  filename: gui.h
  author: dratvsim
*/

#ifndef __GUI_H__
#define __GUI_H__

typedef enum {
  HELP,
  GET_VERSION
} msg_box;

void gui_init(void);
void gui_cleanup(void);
void gui_clear(void);
void gui_refresh(void);

void gui_msg(const char *msg, const char *title);

void *gui_win_thread(void* d);
#endif

/*end of gui.h*/