//
//  recurstats.c
//  recurstats
//
//  Created by Paolo Bosetti on 24/01/2017.
//  Copyright © 2017 Paolo Bosetti. All rights reserved.
//

#include "../include/recurstats.h"
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_cdf.h>

#pragma mark - Static functions
static double t_test(rs_recurstats * rs);
static uint8_t ringbuf_put(rs_recurstats *r, double value);

rs_recurstats * rs_init(uint8_t buflen_pow) {
  uint8_t buflen = pow(2, buflen_pow);
  rs_recurstats * rs =(rs_recurstats *)malloc(sizeof(rs_recurstats));
  rs->depth = 0;
  rs->mean = 0.0;
  rs->stdev = 0.0;
  rs->buffer_data = calloc(buflen, sizeof(double));
  rs->buflen = buflen;
  rs->mask = buflen - 1;
  rs->put_ptr = 0;
  rs->last_ptr = 0;
  return rs;
}

void rs_free(rs_recurstats * rs) {
  free(rs->buffer_data);
  free(rs);
}

void rs_reset(rs_recurstats * rs) {
  rs->depth = 1;
  rs->mean = rs_last_value(rs);
  rs->stdev = 0.0;
}

void rs_add_value(rs_recurstats * rs, double x) {
  ringbuf_put(rs, x);
  if (0 == rs->depth) {
    rs->mean = x;
    rs->stdev = 0.0;
    rs->depth = 1;
  }
  else {
    size_t n = ++rs->depth;
    rs->mean = ((n - 1) * rs->mean + x) / n;
    rs->stdev = sqrt(((n - 2) * pow(rs->stdev, 2) +
                      (double)n / (n - 1) * pow(rs->stdev - x, 2)) / (n - 1));
  }
}

double rs_add_value_and_check(rs_recurstats * rs, double value) {
  rs_add_value(rs, value);
  return t_test(rs);
}

double rs_last_value(rs_recurstats *r) {
  return r->buffer_data[r->last_ptr];
}


// Student's T test on rs->buffer
static double t_test(rs_recurstats * rs) {
  double t0, xbar;
  uint8_t i;
  if (0.0 == rs->stdev)
    return 1.0;
  for (i = 0; i < rs->buflen; i++) {
    xbar += rs->buffer_data[i];
  }
  xbar /= rs->buflen;
  t0 = (rs->mean - xbar) / (rs->stdev/sqrt(rs->depth));
  return gsl_cdf_tdist_Q(fabs(t0), (double)(rs->depth - 1));
}

// Ring buffer
static uint8_t ringbuf_put(rs_recurstats *r, double value) {
  /* return the index of the last value inserted.
   
   NOTE: there is a potential risk for a race condition here, because
   the ->get_ptr field may be written concurrently by the
   ringbuf_get() function. To avoid this, access to ->get_ptr must
   be atomic. We use an uint8_t type, which makes access atomic on
   most platforms, but C does not guarantee this.
   */
  r->buffer_data[r->put_ptr] = value;
  r->last_ptr = r->put_ptr;
  r->put_ptr = (r->put_ptr + 1) & r->mask;
  return r->last_ptr;
}


