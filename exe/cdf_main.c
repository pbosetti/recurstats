//
//  cdf_main.c
//  CDF_test
//
//  Created by Paolo Bosetti on 25/01/2017.
//  Copyright © 2017 Paolo Bosetti. All rights reserved.
//  Testing the rs_cdf_tdist_Q. Generates output code to be loaded in GNU-R.

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "../src/rs_cdf_t.h"

int main(int argc, const char * argv[]) {
  size_t n = 20;
  size_t i, N;
  double t, w;
  N = 100;
  w = 10;
  printf("str <- \"i t cfd\n");
  for (i = 0; i <= N; i++) {
    t = -w + fabs((2*w/(double)N)) * i;
    printf("%zu %f %12.10f\n", i, t, rs_cdf_tdist_Q(t, n));
  }
  printf("\"\ntbl <- read.table(h=T, text=str)\n\n");
  printf("curve(pt(x, %zu, low=F), from=-%f, to=%f )\npoints(cfd~t, data=tbl, col=\"red\")\n\n", n, w, w);
  
  return 0;
}
