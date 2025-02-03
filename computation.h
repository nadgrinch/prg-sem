/*
  filename: computation.h
  author: dratvsim
*/

#ifndef __COMPUTATION_H__
#define __COMPUTATION_H__

#endif
#include <stdbool.h>
#include "messages.h"

void compute_init(void);
void compute_cleanup(void);

void get_grid_size(int *w, int *h);
bool is_computing(void);
bool is_done(void);
bool is_abort(void);
bool is_zoom(void);

void abort_comp(void);
void enable_comp(void);
void reset_cid(void);
void toggle_zoom(void);

bool set_compute(message *msg);
bool compute(message *msg);
void compute_cpu(void);
void compute_cpu_animate(int type);
void compute_cpu_zoom(int x, int y, bool dir);

void update_image(int w, int h, unsigned char *img);
void update_data(const msg_compute_data *compute_data);
int update_parameters(void);

/*end of computation.h*/