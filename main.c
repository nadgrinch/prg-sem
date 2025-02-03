/*
  filename: main.c
  author: dratvsim
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "messages.h"
#include "keyboard.h"
#include "event_queue.h"
#include "computation.h"
#include "gui.h"
#include "utils.h"
#include "xwin_sdl.h"

#include "main.h"

void process_pipe_message(event * const ev);
void print_help(void);

void* main_thread(void* d){
  my_assert(d != NULL, __func__,__LINE__,__FILE__);
  int pipe_out = *(int*)d;
  message msg;
  uint8_t msg_buf[sizeof(message)];
  int msg_len;
  bool quit = false;

  // init computation, visualization
  compute_init();
  gui_init();

  while (!quit) {
    event ev = queue_pop();
    msg.type = MSG_NBR;
    switch (ev.type) {
      case EV_QUIT:
        // quit applicaiton
        set_quit();
        break;
      case EV_GET_VERSION:
        // get version from module
        msg.type = MSG_GET_VERSION;
        break;
      case EV_SET_COMPUTE:
        // set compute param to module
        info(set_compute(&msg) ? "Set Compute OK" : "Set Compute FAIL");
        break;
      case EV_COMPUTE:
        // compute via module
        enable_comp();
        info(compute(&msg) ? "Computing OK" : "Computing FAIL");
        break;
      case EV_ABORT:
        // abort module computation
        if (!is_abort()) {
          abort_comp();
          msg.type = MSG_ABORT;
          info("Abort sent");
        }
        else {
          info("Abort not sent, module not computing");
        }
        break;
      case EV_PIPE_IN_MESSAGE:
        // received msg into pipe_in
        process_pipe_message(&ev);
        break;
      case EV_HELP: 
        // print help msg in terminal
        print_help();
        break;
      case EV_CLEAR_BUFFER:
        // clear compute buffer
        gui_clear();
        info("Computation Buffer cleared");
        break;
      case EV_REFRESH:
        // refresh window with compute buffer
        gui_refresh();
        info("Window Refreshed");
        break;
      case EV_RESET_CHUNK:
        // reset chunk id (cid)
        reset_cid();
        info("Chunk id (cid) reset");
        break;
      case EV_COMPUTE_CPU:
        // compute via CPU (main), refresh after whole img calculated
        compute_cpu();
        info("Computation done by PC");
        break;
      case EV_CHANGE_PARAM:
        // change computation parameters
        int r = update_parameters();
        if (r != -1) {
          info("Parameter change");
          info(set_compute(&msg) ? "new set compute OK" : "new set compute FAIL");
        }
        else {
          info("Parameter change canceled");
        }
        break;
      case EV_ANIMATE: 
        // start animation of compute cpu
        int animate = xwin_animate();
        if (animate != -1) {
          info(animate ? "Animation decrease" : "Animation increase");
          compute_cpu_animate(animate);
        }
        else {
          info("Animation canceled");
        }
        break;
      case EV_ZOOM:
        // toggle zoom
        toggle_zoom();
        info(is_zoom() ? "Zoom Enabled" : "Zoom Disabled");
        break;
      case EV_SAVE_IMG:
        // save img to choosen format
        xwin_save();
      default:
        break;
    }
    if (msg.type != MSG_NBR) {
      my_assert(fill_message_buf(&msg, msg_buf, sizeof(message), &msg_len), __func__, __LINE__, __FILE__);
      if (write(pipe_out, msg_buf, msg_len) == msg_len) {
        // debug("Send message to pipeout OK");
      }
      else {
        error("Send message to pipeout Fail");
      }
    }
    quit = is_quit();
  }
  // cleanup computation, visualization
  compute_cleanup();
  gui_cleanup();
  info("main thread - finish");
  return NULL;
}

void process_pipe_message(event * const ev){
  my_assert( ev != NULL && ev->type == EV_PIPE_IN_MESSAGE && ev->data.msg, __func__,__LINE__,__FILE__);
  ev->type = EV_TYPE_NUM;
  const message *msg = ev->data.msg;
  switch (msg->type) {
    case MSG_OK:
      info("Received OK");
      break;
    case MSG_ERROR:
      error("Received Error");
      break;
    case MSG_STARTUP:
      fprintf(stderr, "Startup message %s\n",msg->data.startup.message);
      gui_msg((char *)msg->data.startup.message,"Startup message");
      break;
    case MSG_VERSION:
      char text[13];
      sprintf(text, "%d.%d-p%d", msg->data.version.major, msg->data.version.minor, msg->data.version.patch);
      fprintf(stderr, "INFO: Module version %d.%d-p%d\n", msg->data.version.major, msg->data.version.minor, msg->data.version.patch);
      gui_msg(text, "Module Version");
      break;
    case MSG_COMPUTE_DATA:
      update_data(&(msg->data.compute_data));
      break;
    case MSG_DONE:
      gui_refresh();
      if (is_done()){
        info("Computation done");
      }
      else {
        event ev = { .type = EV_COMPUTE};
        queue_push(ev);
      }
      break;
    case MSG_ABORT:
      info("Abort received");
      abort_comp();
      break;
    default:
      fprintf(stderr, "Unhandled pipe message %d\n",msg->type);
      break;
  }
  free(ev->data.msg);
  ev->data.msg = NULL;
}

void print_help(void){
  const char *help_msg = 
  "\n"
  "------------HELP------------\n\r"
  "| keybinds for control      |\n\r"
  "| g - get version           |\n\r"
  "| s - set compute           |\n\r"
  "| 1 - compute via module    |\n\r"
  "| c - compute via cpu       |\n\r"
  "| a - abort ongoing compute |\n\r"
  "| r - reset chunk id (cid)  |\n\r"
  "| l - clear buffer          |\n\r"
  "| p - refresh window        |\n\r"
  "| x - change parameters     |\n\r"
  "| m - start animation       |\n\r"
  "| z - toggle zoom           |\n\r"
  "| mouse left - zoom in      |\n\r"
  "| mouse right - zoom out    |\n\r"
  "| q - quit application      |\n\r"
  "----------------------------\n\r";
  info(help_msg);
}

/*end of main.c*/