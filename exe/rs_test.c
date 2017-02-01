//
//  main.c
//  recurstats
//
//  Created by Paolo Bosetti on 24/01/2017.
//  Copyright © 2017 Paolo Bosetti. All rights reserved.
//

#include <stdio.h>
#include "../include/recurstats.h"
#include "../src/rs_cdf_t.h"

#define N 200
#define ALPHA 0.005
#define BUFLEN 3
int main(int argc, const char * argv[]) {
  rs_recurstats *rs = rs_init(BUFLEN);
  uint8_t i, j;
  double p, d;
  // this program prints out a valid GNU-R script for plotting results.
  
  // Header
  printf("tbl <- read.table(h=T, text=\"\n");
  printf("%3s %3s ", "n", "dep");
  for (j = 0; j<rs->buflen; j++)
    printf("%8s%02d ", "b", j);
  printf("%10s %10s %10s %10s %s\n", "last", "mean", "sd", "p-value", "r");
  
  // Body
  for (i = 0; i < N; i++) {
    if (i < N / 2)
      d = box_muller(10, 0.5);
    else
      d = box_muller(15, 1.0);
    p = rs_add_value_and_check(rs, d);
    printf("%3d %3zu ", i, rs->depth);
    for (j = 0; j<rs->buflen; j++)
      printf("%10.6f ", rs->buffer_data[j]);
    printf("%10.6f %10.6f %10.6f %10.6f %1s\n", rs_last_value(rs), rs->mean, rs->stdev, p, p < ALPHA ? "Y" : "-");
    if (p < ALPHA) {
      rs_reset(rs);
    }
  }
  
  // Footer
  printf("\")\nplot(tbl$last, ylab=\"values\")\nlines(tbl$mean, col=\"red\")\nlines(tbl$mean+tbl$sd, col=\"green\")\nlines(tbl$mean-tbl$sd, col=\"green\")\nabline(v=which(tbl$r==\"Y\"), col=\"darkgray\", lty=2)\n");
  rs_free(rs);
  return 0;
}
