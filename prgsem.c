/*
  filename: prgsem.c
  assignment: https://cw.fel.cvut.cz/wiki/courses/b3b36prg/semestral-project/start
  author: dratvsim

  author note: base code structure is inspired by video tutorial
  https://www.youtube.com/playlist?list=PLQ5Wg6tJelyuJHNwv40B7qkH69i7tZycv 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "utils.h"
#include "prg_io_nonblock.h"
#include "main.h"
#include "keyboard.h"
#include "event_queue.h"
#include "gui.h"

#ifndef IO_READ_TIMEOUT_MS
#define IO_READ_TIMEOUT_MS 100
#endif

void* read_pipe_thread(void*);

int main(int argc, char const *argv[])
{
  int ret = EXIT_SUCCESS;
  const char *fname_pipe_in = argc > 1 ? argv[1] : "/tmp/computational_module.out";
  const char *fname_pipe_out = argc > 2 ? argv[2] : "/tmp/computational_module.in";

  int pipe_in = io_open_read(fname_pipe_in);
  int pipe_out = io_open_read(fname_pipe_out);
  pipe_out = io_open_write(fname_pipe_out);
  
  my_assert(pipe_in != -1 && pipe_out != -1, __func__, __LINE__, __FILE__);

  enum { KEYBOARD_THRD, READ_PIPE_THRD, MAIN_THRD, GUI_THRD, NUM_THREADS };
  const char *thrds_name[] = { "Keyboard", "ReadPipe", "Main","GuiWin"};
  
  void* (*thrd_functions[])(void*) = {keyboard_thread, read_pipe_thread, main_thread, gui_win_thread};
  pthread_t threads[NUM_THREADS];
  void* thrd_data[NUM_THREADS] = {};
  thrd_data[READ_PIPE_THRD] = &pipe_in;
  thrd_data[MAIN_THRD] = &pipe_out;
  
  for (int i = 0; i < NUM_THREADS; ++i){
    int r = pthread_create(&threads[i], NULL, thrd_functions[i], thrd_data[i]);
    printf("INFO: Create thread '%s' %s\r\n", thrds_name[i], ( r == 0 ? "OK" : "FAIL"));
  }

  int *ex;
  for (int i = 0; i < NUM_THREADS; ++i){
    printf("INFO: Call join to the thread %s\r\n", thrds_name[i]);
    int r = pthread_join(threads[i], (void*)&ex);
    printf("INFO: Joining the thread %s has been %s\r\n", thrds_name[i], (r == 0 ? "OK" : "FAIL"));
  }

  io_close(pipe_in);
  io_close(pipe_out);

  return ret;
}

void *read_pipe_thread(void* d){
  my_assert(d != NULL, __func__,__LINE__,__FILE__);
  int pipe_in = *(int*)d;
  info("read_pipe_thread - start");
  
  bool end = false;
  uint8_t msg_buf[sizeof(message)];
  int i = 0;
  int len = 0;

  unsigned char c;
  // discrad garbage
  while (io_getc_timeout(pipe_in, IO_READ_TIMEOUT_MS, &c) > 0) {};

  while(!end) {
    int r = io_getc_timeout(pipe_in, IO_READ_TIMEOUT_MS, &c);
    if (r > 0) {
      // char has been read
      if (i == 0) {
        len = 0;
        if (get_message_size(c, &len)) {
          msg_buf[i++] = c;
        }
        else {
          fprintf(stderr, "ERROR: uknown message type\n");
        }
      }
      else {
        // read remaining bytes of msg
        msg_buf[i++] = c;
      }
      if (len > 0 && i == len) {
        message *msg = my_alloc(sizeof(message));
        if (parse_message_buf(msg_buf, len, msg)) {
          event ev = { .type = EV_PIPE_IN_MESSAGE };
          ev.data.msg = msg;
          queue_push(ev);
        }
        else {
          fprintf(stderr, "ERROR: cannot parse message type\n");
          free(msg);
        }
        i = len = 0;
      }
    }
    else if (r == 0){
      // timeout
    }
    else {
      // error, handle error
      error("reading from pipe");
      set_quit();
      event ev = { .type = EV_QUIT};
      queue_push(ev);
    }
    
    end = is_quit();
  }

  info("read_pipe_thread - finish");
  return NULL;
}