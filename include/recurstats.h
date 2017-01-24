//
//  recurstats.h
//  recurstats
//
//  Created by Paolo Bosetti on 24/01/2017.
//  Copyright © 2017 Paolo Bosetti. All rights reserved.
//

#ifndef recurstats_h
#define recurstats_h
#include <sys/types.h>
#include <stdint.h>

typedef struct {
  double mean;
  double stdev;
  size_t depth;
  // circular buffer
  uint8_t buflen;
  double *buffer_data;
  uint8_t mask;
  /* XXX these must be 8-bit quantities to avoid race conditions. */
  uint8_t put_ptr, last_ptr;

} rs_recurstats;

rs_recurstats * rs_init(size_t buflen);
void rs_free(rs_recurstats * rs);

void rs_reset(rs_recurstats * rs);

void rs_add_value(rs_recurstats * rs, double value);

double rs_add_value_and_check(rs_recurstats * rs, double value);

double rs_last_value(rs_recurstats *rs);




#endif /* recurstats_h */
