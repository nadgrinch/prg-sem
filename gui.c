/*
  filename: gui.c
  author: dratvsim
*/

#include <SDL2/SDL.h>

#include "xwin_sdl.h"
#include "utils.h"
#include "computation.h"
#include "event_queue.h"

#include "gui.h"

#ifndef SDL_EVENT_POLL_WAIT_MS
#define SDL_EVENT_POLL_WAIT_MS 10
#endif

static struct {
  int w;
  int h;
  unsigned char *img;
} gui = { .img = NULL };

static char *help_msg = 
"     Keybinds\n\n"
" G - GET VERSION\n\n"
" S - SET COMPUTE\n\n"
" 1 - COMPUTE VIA MODULE\n\n"
" C - COMPUTE VIA PC\n\n"
" A - ABORT ONGOING COMPUTE\n\n"
" R - RESET CHUNK ID (cid)\n\n"
" L - CLEAR BUFFER\n\n"
" P - REFRESH WINDOW\n\n"
" X - CHANGE PARAMETERS\n\n"
" M - START ANIMATION\n\n"
" Z - TOGGLE ZOOM\n\n"
" MOUSE LEFT - ZOOM IN\n\n"
" MOUSE RIGHT - ZOOM OUT\n\n"
" J - SAVE IMAGE\n\n"
" Q - QUIT";

static char *welcome_msg = 
"Welcome to Semestral work by Šimon Dratva\n"
"-----------------------------------------\n"
"PRESS H for help with keybinds";

void gui_init(void){
  get_grid_size(&gui.w, &gui.h);
  gui.img = my_alloc(gui.w * gui.h * 3);
  my_assert(xwin_init(gui.w,gui.h) == 0, __func__,__LINE__,__FILE__);
}

void gui_cleanup(void){
  if (gui.img) {
    free(gui.img);
    gui.img = NULL;
  }
  xwin_close();
}

void gui_clear(void){
  if (gui.img) {
    compute_cleanup();
    compute_init();
    for (int i = 0; i < gui.h * gui.w * 3; ++i) {
      gui.img[i] = 0;
    }
    xwin_redraw(gui.w, gui.h, gui.img);
  }
}

void gui_refresh(void){
  if (gui.img) {
    update_image(gui.w, gui.h, gui.img);
    xwin_redraw(gui.w, gui.h, gui.img);
  }
}

void gui_msg(const char *msg, const char *title){
  int r = xwin_msg(msg,title);
  if (r != 0) {
    error("Gui Message Box error");
    event ev = { .type = EV_QUIT };
    queue_push(ev);
  }
}

void handle_keyevent(SDL_Event ev){
  event ret = { .type = EV_TYPE_NUM };
  switch(ev.key.keysym.sym) {
    case SDLK_q:
      // quit window
      ret.type = EV_QUIT;
      break;
    case SDLK_h:
      // help menu
      gui_msg(help_msg,"Nápověda");
      break;
    case SDLK_s:
      // set compute parameters
      ret.type = EV_SET_COMPUTE;
      break;
    case SDLK_c:
      // compute via pc (main)
      ret.type = EV_COMPUTE_CPU;
      break;
    case SDLK_a:
      // abort compute
      ret.type = EV_ABORT;
      break;
    case SDLK_1:
      // start compute
      ret.type = EV_COMPUTE;
      break;
    case SDLK_g:
      // get version from module
      ret.type = EV_GET_VERSION;
      break;
    case SDLK_r:
      // reset chunk id (cid)
      ret.type = EV_RESET_CHUNK;
      break;
    case SDLK_p:
      // refresh surface with comp buffer
      gui_refresh();
      info("Window Refreshed");
      break;
    case SDLK_l:
      // clear computation buffer
      gui_clear();
      info("Computation Buffer cleared");
      break;
    case SDLK_x:
      // parameter change menu
      ret.type = EV_CHANGE_PARAM;
      break;
    case SDLK_m:
      // animation menu
      ret.type = EV_ANIMATE;
      break;
    case SDLK_z:
      // toggle zoom
      ret.type = EV_ZOOM;
      break;
    case SDLK_j:
      // save menu open
      ret.type = EV_SAVE_IMG;
      break;
    default:
      // fprintf(stderr, "DEBUG: %d key pressed\n",ev.key.keysym.sym);
      break;
  }
  if (ret.type != EV_TYPE_NUM) {
    queue_push(ret);
  }
}

void handle_windowevent(SDL_Event ev){
  event ret = { .type = EV_TYPE_NUM };
  switch (ev.window.event) {
    case SDL_WINDOWEVENT_CLOSE:
      ret.type = EV_QUIT;
      break;
    default:
      break;
  }
  if (ret.type != EV_TYPE_NUM) {
    queue_push(ret);
  }
}

void handle_mouseevent(SDL_Event ev){
  switch (ev.button.button) {
    case SDL_BUTTON_LEFT:
      info("Zooming in");
      // fprintf(stderr,"coords: (%d,%d)\n",ev.button.x,ev.button.y);
      compute_cpu_zoom(ev.button.x,ev.button.y,true); 
      break;
    case SDL_BUTTON_RIGHT:
      info("Zooming out");
      // fprintf(stderr,"coords: (%d,%d)\n",ev.button.x,ev.button.y);
      compute_cpu_zoom(ev.button.x,ev.button.y,false);
    default:
      break;
  }
}

void* gui_win_thread(void* d){
  info("gui_win_thread start");
  SDL_Delay(500);
  gui_msg(welcome_msg, "Welcome");
  bool quit = false;
  SDL_Event ev;
  while (!quit) {
    if (SDL_PollEvent(&ev)) {
      switch (ev.type) {
        case SDL_KEYDOWN:
          handle_keyevent(ev);
          break;
        case SDL_WINDOWEVENT:
          handle_windowevent(ev);
          break;
        case SDL_MOUSEBUTTONUP:
          handle_mouseevent(ev);
          break;
      } 
    }
    SDL_Delay(SDL_EVENT_POLL_WAIT_MS);
    quit = is_quit();
  }
  info("gui_win_thread finish");
  return NULL;
}

/*end of gui.c*/