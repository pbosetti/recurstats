Recursive (running) statistics
==============================

A rather simple and easy to use C library for performing running statistics on a sequence of incoming data. 

The library is designed to minimize memory footprint. One data sample at a time is added, then mean and standard deviation are incrementally calculated on the basis of the statistics (indicators) value of the previous iteration. See the docs for mathematical treatment.

Together with the running indicators, a small (and configurable) ring buffer of the previous 2^n samples is regularly updated. This allows to perform a Student's T-test comparing the average of the sample buffer with the running (long-term) average. If the data are shifting, the p-value of the t-test becomes smaller. Below a certain probability threshold, you might want to reset the running average. 