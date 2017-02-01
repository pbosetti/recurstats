//
//  recurstats.c
//  recurstats
//
//  Created by Paolo Bosetti on 24/01/2017.
//  Copyright © 2017 Paolo Bosetti. All rights reserved.
//

#include <stdlib.h>
#include <math.h>
#include "../include/recurstats.h"
#include "rs_cdf_t.h"

#pragma mark - Static functions
static double t_test(rs_recurstats * rs);
static uint8_t ringbuf_put(rs_recurstats *r, double value);

static double mean(const double *data, const size_t size) {
  long double mean = 0;
  size_t i;

  for (i = 0; i < size; i++) {
    mean += (data[i] - mean) / (i + 1);
  }

  return mean;
}

static double stdev(const double *data, const size_t n, const double mean) {
  long double tss = 0;
  size_t i;

  for (i = 0; i < n; i++) {
    const long double delta = (data[i] - mean);
    tss += delta * delta;
  }

  return sqrt(tss / (n - 1));
}


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
  rs->mean = rs_last_value(rs);
  rs->stdev = 0.0;
  rs->depth = 1;
}

void rs_add_value(rs_recurstats * rs, double x) {
  ringbuf_put(rs, x);
  switch (rs->depth) {
    case -1:
      // overflow: keep everything constant
      break;
      
    case 0:
      rs->mean = x;
      rs->stdev = 0.0;
      rs->depth = 1;
      break;
      
    default: {
      size_t n = ++rs->depth;
      rs->mean = ((n - 1) * rs->mean + x) / n;
      rs->stdev = sqrt(((n - 2) * pow(rs->stdev, 2) +
                        (double)n / (n - 1) * pow(rs->mean - x, 2)) / (n - 1));

      break;
    }
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
  double t0, xbar, sd, result;
  if (0.0 == rs->stdev)
    return 0.5;
  if (rs->depth <= 2 * rs->buflen)
    return 0.5;
  xbar = mean(rs->buffer_data, rs->buflen);
  sd = stdev(rs->buffer_data, rs->buflen, xbar);
  t0 = (rs->mean - xbar) / (sd/sqrt(rs->buflen));
  result = rs_cdf_tdist_Q(fabs(t0), (double)(rs->buflen - 1)) * 2.0;
  return result;
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


