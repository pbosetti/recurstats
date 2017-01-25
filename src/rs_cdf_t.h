//
//  cdf_t.h
//  recurstats
//
//  Created by Paolo Bosetti on 25/01/2017.
//  Copyright © 2017 Paolo Bosetti. All rights reserved.
//

#ifndef rs_cdf_t_h
#define rs_cdf_t_h

// Upper tail
double rs_cdf_tdist_Q(double t0, double n);

// Lower tail
double rs_cdf_tdist_P(double t0, double n);

#endif /* rs_cdf_t_h */
