#include <stdint.h>
#include <stdio.h>

/* Module to digitally process wav signals using a variety of filters */

/* Delay Line ***************************************************************/

typedef struct delay_line {

	float *buffer;
	size_t count;			// i'll take the biggest int you got
	size_t delay_length;

} delay_line;

delay_line* init_delay_line(size_t delay_length);
/* initialize delay line struct, defining the delay length, buffer length, 
   and the setting the count to 0. */

void step_delay_line(delay_line *DL, float input, float *output);
/* sends the input data through the delay line (operating more or less like 
   a circular buffer) and outputs the appropriate data based on the 
   delay_length. If delay_length = 3, the output will equal 0 for the first 
   three iterations. */

void delete_delay_line(delay_line *DL);
/* free all memory, clear all buffers... you know that sort of stuff. */

/* Delay Line End ***********************************************************/

/* Feed-Forward Comb Filter *************************************************/

typedef struct ff_comb_filter {

	float b0;
	float bm;
	delay_line *DL;

} ff_comb_filter;

ff_comb_filter* init_ff_comb_filter(size_t delay_length, float b0, float bm);
/* initialize feed-forward comb filter, defining the delay length and the 
   scalar consants b0 and bm. */

void step_ff_comb_filter(ff_comb_filter *FFCF, float input, float *output);
/* sends the input data through the comb filter, using the delay line and 
   scalar constants, and outputs the filtered data. */

void delete_ff_comb_filter(ff_comb_filter *FFCF);
/* free all memory, clear all buffers... you know that sort of stuff. */

/* Feed-Forward Comb Filter End *********************************************/

void convolution_reverb(float* input, float* impulse, float* output, 
						int input_length, int impulse_length);

