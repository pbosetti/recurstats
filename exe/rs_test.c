//
//  main.c
//  recurstats
//
//  Created by Paolo Bosetti on 24/01/2017.
//  Copyright © 2017 Paolo Bosetti. All rights reserved.
//

#include <stdio.h>
#include "../include/recurstats.h"
#define N 12

int main(int argc, const char * argv[]) {
  rs_recurstats *rs = rs_init(2);
  uint8_t i, j, n;
  double p;
  n = N;
  double data[N] = {
    0.3, 1.4, 2.1, 3.7,
    4.9, 5.6, 60.1, 61.7,
    62.2, 63.5, 64.1, 65.4
  };
  
  for (i = 0; i < n; i++) {
    p = rs_add_value_and_check(rs, data[i]);
    for (j = 0; j<rs->buflen; j++)
      printf("%f ", rs->buffer_data[j]);
    printf("- Last: %f, %f, %f\n", rs_last_value(rs), rs->mean, p);
    if (p < 0.01) {
      printf("Reset!\n");
      rs_reset(rs);
    }
  }

  rs_free(rs);
  return 0;
}
