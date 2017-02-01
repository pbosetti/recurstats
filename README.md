Recursive (running) statistics
==============================

A rather simple and easy to use C library for performing running statistics on a sequence of incoming data. 

The library is designed to minimize memory footprint. One data sample at a time is added, then mean and standard deviation are incrementally calculated on the basis of the statistics (indicators) value of the previous iteration. See the docs for mathematical treatment.

Together with the running indicators, a small (and configurable) ring buffer of the previous 2^n samples is regularly updated. This allows to perform a Student's T-test comparing the average of the sample buffer with the running (long-term) average. If the data are shifting, the p-value of the t-test becomes smaller. Below a certain probability threshold, you might want to reset the running average.

Build
-----

Compile it with the provided Xcode project. It generates a static library.

If you prefer, or if you ar not on a Mac, you can use the provided Cmake facility:

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```


Usage
-----

See example in `exe/rs_test.c`:

```C
int main(int argc, const char * argv[]) {
  rs_recurstats *rs = rs_init(2); // Initialize a new instance rs
  uint8_t i, j, n;
  double p;
  n = N;
  double data[N] = {  // Some data fro testing
    0.3, 1.4, 2.1, 3.7,
    4.9, 5.6, 60.1, 61.7,
    62.2, 63.5, 64.1, 65.4
  };
  
  for (i = 0; i < n; i++) {
    // Adds a new point and perform the Student's T-test on the ring
    // buffer content. A small probability p means that 
    // the data is drifting and we shall reset the running stats
    p = rs_add_value_and_check(rs, data[i]); 
    for (j = 0; j<rs->buflen; j++)
      printf("%f ", rs->buffer_data[j]);
    printf("- Last: %f, %f, %f\n", rs_last_value(rs), rs->mean, p);
    if (p < 0.01) {
      printf("Reset!\n");
      rs_reset(rs); // reset the running stats
    }
  }
  
  // Remember to deallocate rs
  rs_free(rs);
  return 0;
}

```

The current running statistics, mean and standard deviation, are available as `rs->mean` and `rs->std`, respectively.
**NOTE:** the ring buffer must have a size that is a power of 2. For this reason, the initializer `rs_init()` takes as argument the **exponent** of the buffer size, i.e. `rs_init(4)` creates a buffer of 2^4 elements.

Example
-------
The following chart shows the results of applying the `rs_add_value_and_check()` function to a generation of 200 random normal data, where the first 100 points have mean 10 and standard deviation 0.5, the last 100 points have mean 15 and standard deviation 1.

![Example chart](https://raw.githubusercontent.com/pbosetti/recurstats/master/doc/example.png)
