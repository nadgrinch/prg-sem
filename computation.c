/*
  filename: computation.c
  author: dratvsim
*/
#include <stdio.h>
#include "computation.h"
#include "xwin_sdl.h"
#include "event_queue.h"
#include "utils.h"

static struct {
  double c_re;
  double c_im;
  int n;

  double range_re_min;
  double range_re_max;
  double range_im_min;
  double range_im_max;

  int grid_w;
  int grid_h;

  int cur_x;
  int cur_y;

  double d_re;
  double d_im;

  int nbr_chunk;
  int cid;
  double chunk_re;
  double chunk_im;

  uint8_t chunk_n_re;
  uint8_t chunk_n_im;

  uint8_t *grid;
  bool computing;
  bool abort;
  bool zoom;
  bool done;

} comp = {
    .c_re = -0.4,
    .c_im =  0.6,
    .n = 60,
    
    .range_re_min = -1.6,
    .range_re_max =  1.6,
    .range_im_min = -1.1,
    .range_im_max =  1.1,
    
    .grid = NULL,
    .grid_w = 640,
    .grid_h = 480,

    .chunk_n_re = 64,
    .chunk_n_im = 48,

    .computing = false,
    .abort = false,
    .zoom = false,
    .done = false,

};

void compute_init(void){
  comp.grid = my_alloc(comp.grid_w * comp.grid_h);
  for (int i = 0; i < comp.grid_w * comp.grid_h; ++i) {
    comp.grid[i] = 0;
  }
  comp.d_re = (comp.range_re_max - comp.range_re_min) / (1. * comp.grid_w);
  comp.d_im = -(comp.range_im_max - comp.range_im_min) / (1. * comp.grid_h);
  comp.nbr_chunk = (comp.grid_w * comp.grid_h) / (comp.chunk_n_re * comp.chunk_n_im);

}
void compute_cleanup(void){
  if (comp.grid) {
    free(comp.grid);
  }
  comp.grid = NULL;
}

bool is_computing(void){ return comp.computing; }
bool is_done(void){ return comp.done; }
bool is_abort(void) { return comp.abort; }
bool is_zoom(void) { return comp.zoom; }

void get_grid_size(int *w, int *h){
  *w = comp.grid_w;
  *h = comp.grid_h;
}

void abort_comp(void){ comp.abort = true;}
void enable_comp(void){ comp.abort = false;}
void reset_cid(void){ comp.computing = false;}
void toggle_zoom(void) {comp.zoom = !comp.zoom; }

bool set_compute(message *msg){
  my_assert(msg != NULL, __func__,__LINE__,__FILE__);
  bool ret = !is_computing();
  if (ret) {
    msg->type = MSG_SET_COMPUTE;
    msg->data.set_compute.c_re = comp.c_re;
    msg->data.set_compute.c_im = comp.c_im;
    msg->data.set_compute.d_re = comp.d_re;
    msg->data.set_compute.d_im = comp.d_im;
    msg->data.set_compute.n = comp.n;
    comp.done = false;

  }
  return ret;
}

bool compute(message *msg){
  if (!is_computing()) { // first chunk
    comp.cid = 0;
    comp.computing = true;
    comp.done = false;

    comp.cur_x = comp.cur_y = 0;
    comp.chunk_re = comp.range_re_min; //upper left corner
    comp.chunk_im = comp.range_im_max; //upper left corner
    msg->type = MSG_COMPUTE;
  }
  else { // next chunk
    comp.cid += 1;
    if (comp.cid < comp.nbr_chunk){
      comp.cur_x += comp.chunk_n_re;
      comp.chunk_re += comp.chunk_n_re * comp.d_re;
      if (comp.cur_x >= comp.grid_w) {
        comp.chunk_re = comp.range_re_min;
        comp.chunk_im += comp.chunk_n_im * comp.d_im;
        
        comp.cur_x = 0;
        comp.cur_y += comp.chunk_n_im;
      }
      msg->type = MSG_COMPUTE;
    }
    else { // all has been computed

    }
  }

  if (comp.computing && msg->type == MSG_COMPUTE){
    msg->data.compute.cid = comp.cid;
    msg->data.compute.re = comp.chunk_re;
    msg->data.compute.im = comp.chunk_im;
    msg->data.compute.n_re = comp.chunk_n_re;
    msg->data.compute.n_im = comp.chunk_n_im;
  }
  fprintf(stderr,"INFO: Requesting computation of cid: %d\n",comp.cid);
  return is_computing();
}

void compute_cpu(){
  unsigned char *img = (unsigned char*) my_alloc(comp.grid_w * comp.grid_h * 3);
  int idx = 0;
  for (int x = 0; x < comp.grid_h; ++x) {
    for (int y = 0; y < comp.grid_w; ++y) {
      double n_re = comp.range_re_min + y*comp.d_re;
      double n_im = comp.range_im_max + x*comp.d_im;
      double old_re, old_im;
      int i = 0;
      while (i < comp.n && n_re*n_re + n_im*n_im < 4){
        // z_(i+1) = z^2_i + c
        old_re = n_re;
        old_im = n_im;
        n_re = old_re * old_re - old_im * old_im + comp.c_re;
        n_im = 2 * old_re * old_im + comp.c_im;
        i++;
      }
      double t = (double)i / (comp.n + 1.0);
      img[idx++] = 9 * (1-t)*t*t*t * 255;
      img[idx++] = 15 * (1-t)*(1-t)*t*t * 255;
      img[idx++] = 8.5 * (1-t)*(1-t)*(1-t)*t * 255;
    }
  }
  xwin_redraw(comp.grid_w, comp.grid_h, img);
}

void compute_cpu_animate(int type){
  for (int i = 0; i < 100; ++i) {
    compute_cpu();
    switch (type) {
      case 0:
        // increase
        comp.c_re += 0.002;
        comp.c_im += 0.002;
        break;
      case 1:
        // decrease
        comp.c_re -= 0.002;
        comp.c_im -= 0.002;
        break;
    }
  }
}

void compute_cpu_zoom(int x, int y, bool dir){
  if (comp.zoom) {
    // coords to C plane values
    double x_re = comp.range_re_min + x*comp.d_re;
    double y_im = comp.range_im_max + y*comp.d_im;

    // remember old ranges
    double old_r_max = comp.range_re_max;
    double old_r_min = comp.range_re_min;
    double old_i_max = comp.range_im_max;
    double old_i_min = comp.range_im_min;
    double old_r_range = old_r_max - old_r_min;
    double old_i_range = old_i_min - old_i_max;

    if (dir) {
      // zoom in
      comp.range_re_max = x_re + old_r_range/4.0;
      comp.range_re_min = x_re - old_r_range/4.0;
      comp.range_im_max = y_im - old_i_range/4.0;
      comp.range_im_min = y_im + old_i_range/4.0;
    }
    else {
      // zoom out
      comp.range_re_max = x_re + old_r_range;
      comp.range_re_min = x_re - old_r_range;
      comp.range_im_max = y_im - old_i_range;
      comp.range_im_min = y_im + old_i_range;
    }
    comp.d_re = (comp.range_re_max - comp.range_re_min) / (1.0 * comp.grid_w);
    comp.d_im = (comp.range_im_min - comp.range_im_max) / (1.0 * comp.grid_h);
    compute_cpu();
  }
  else {
    warn("Zoom not enabled");
  }

}

void update_image(int w, int h, unsigned char *img){
  my_assert(img && comp.grid && w == comp.grid_w && h == comp.grid_h, __func__, __LINE__, __FILE__);
  for (int i = 0; i < w * h; ++i) {
    const double t = 1. *comp.grid[i] / (comp.n + 1.0);
    *(img++) = 9 * (1-t)*t*t*t * 255;
    *(img++) = 15 * (1-t)*(1-t)*t*t * 255;
    *(img++) = 8.5 * (1-t)*(1-t)*(1-t)*t * 255;
  }
}

void  update_data(const msg_compute_data *compute_data){
  my_assert(compute_data != NULL, __func__,__LINE__,__FILE__);
  if (compute_data->cid == comp.cid) {
    const int idx = comp.cur_x + compute_data->i_re + (comp.cur_y + compute_data->i_im) * comp.grid_w;
    if (idx >= 0 && idx < (comp.grid_w * comp.grid_h)){
      comp.grid[idx] = compute_data->iter;
    }
    if ((comp.cid + 1) >= comp.nbr_chunk && (compute_data->i_re + 1) == comp.chunk_n_re && (compute_data->i_im + 1) == comp.chunk_n_im) {
      comp.done = true;
      comp.computing = false;
    }
  }
  else {
    warn("Received chunk with unexpected chunk id (cid)");
  }
}


int update_parameters(void){
  bool end = false;
  int buttonid = -1;
  while (!end) {
    buttonid = xwin_parameters();
    switch (buttonid) {
      case 0: // re max
        xwin_change_param(buttonid,&comp.range_re_max);
        break;
      case 1: // re min
        xwin_change_param(buttonid,&comp.range_re_min);
        break;
      case 2: // im max
        xwin_change_param(buttonid,&comp.range_im_max);
        break;
      case 3: // im min
        xwin_change_param(buttonid,&comp.range_im_min);
        break;
      case 4: // c_re
        xwin_change_param(buttonid,&comp.c_re);
        break;
      case 5: // c_im
        xwin_change_param(buttonid,&comp.c_im);
        break;
      case 6: // n
        xwin_change_n(&comp.n);
        break;
      default:
        end = true;
        break;
    }
  }
  return buttonid;
}
/*end of computation.c*/