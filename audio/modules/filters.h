#include <stdint.h>
#include <stdio.h>

/* Module to digitally process wav signals using a variety of filters */

//void comb_filter(struct data_info* d, int_fast32_t* data); 
/* Basic feedforward comb filter using the equation shown on:
   https://ccrma.stanford.edu/~jos/pasp/Feedforward_Comb_Filters.html */

void convolution_reverb(float* input, float* impulse, float* output, 
						int input_length, int impulse_length);

