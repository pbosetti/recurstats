//
//  cdf_t.c
//  recurstats
//
//  Created by Paolo Bosetti on 25/01/2017.
//  Copyright © 2017 Paolo Bosetti. All rights reserved.
//  According to ACM Algorithm number 395 and 209

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include "rs_cdf_t.h"

static double Gauss(double z) {
  // input = z-value (-inf to +inf)
  // output = p under Normal curve from -inf to z
  // e.g., if z = 0.0, function returns 0.5000
  // ACM Algorithm #209
  double y; // 209 scratch variable
  double p; // result. called 'z' in 209
  double w; // 209 scratch variable
  
  if (z == 0.0)
    p = 0.0;
  else {
    y = fabs(z) / 2.0;
    if (y >= 3.0) {
      p = 1.0;
    }
    else if (y < 1.0) {
      w = y * y;
      p = ((((((((0.000124818987 * w
                  - 0.001075204047) * w + 0.005198775019) * w
                - 0.019198292004) * w + 0.059054035642) * w
              - 0.151968751364) * w + 0.319152932694) * w
            - 0.531923007300) * w + 0.797884560593) * y * 2.0;
    }
    else {
      y = y - 2.0;
      p = (((((((((((((-0.000045255659 * y
                       + 0.000152529290) * y - 0.000019538132) * y
                     - 0.000676904986) * y + 0.001390604284) * y
                   - 0.000794620820) * y - 0.002034254874) * y
                 + 0.006549791214) * y - 0.010557625006) * y
               + 0.011630447319) * y - 0.009279453341) * y
             + 0.005353579108) * y - 0.002141268741) * y
           + 0.000535310849) * y + 0.999936657524;
    }
  }
  
  if (z > 0.0)
    return (p + 1.0) / 2.0;
  else
    return (1.0 - p) / 2.0;
} // Gauss()


double rs_cdf_tdist_Q(double t0, double n) {
  double a, b, y, z, t, student;
  if (n < 1) return -1;
//  if (t0 <= DBL_EPSILON && t0 >= -DBL_EPSILON) return 0.5;

  z = 1.0;
  t = t0 * t0;
  y = t / n;
  b = 1.0 + y;
  
  if (fmod(n, 1) != 0 || (n >= 20 && t < n) || n > 200) {
    // Asymptotic series for large or non-integer n
    if (y > 1E-6) y = log(b);
    a = n - 0.5;
    b = 48.0 * a * a;
    y = a * y;
    y = (((((-0.4 * y - 3.3) * y - 24.0) * y - 85.5) /
          (0.8 * y * y + 100.0 + b) +
          y + 3.0) / b + 1.0) * sqrt(y);
    student = Gauss(-y);
  }
  else {
    if (n < 20 && t < 4) {
      // Nested summation of cosine series
      a = sqrt(y);
      y = sqrt(y);
      if (n == 1) a = 0.0;
    }
    else {
      // tail series expansion for large t values
      size_t j;
      a = sqrt(b);
      y = a * n;
      for (j = 2; a != z; j+=2) {
        z = a;
        y = y * (j - 1)/(b * j);
        a = a + y / (n + j);
      }
      n += 2;
      z = 0.0;
      y = 0.0;
      a = -a;
    }
    
    while ((n-=2) > 1) {
      a = (n - 1) / (b * n) * a + y;
    }
    a = n == 0 ? a / sqrt(b) : (atan(y) + a/b) * M_2_PI;
    student = (z - a) / 2.0;
  }
  return t0 > 0 ? student : 1 - student;
}

double rs_cdf_tdist_P(double t0, double n) {
  return 1 - rs_cdf_tdist_Q(t0, n);
}




double box_muller(float m, float s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
  double x1, x2, w, y1;
  static double y2;
  static int use_last = 0;
  
  if (use_last)		        /* use value from previous call */
  {
    y1 = y2;
    use_last = 0;
  }
  else
  {
    do {
      x1 = 2.0 * ((double) rand() / (RAND_MAX)) - 1.0;
      x2 = 2.0 * ((double) rand() / (RAND_MAX)) - 1.0;
      w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    use_last = 1;
  }
  
  return ( m + y1 * s );
}
