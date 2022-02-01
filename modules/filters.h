#ifndef FILTERS_H_	// include guard
#define FILTERS_H_

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "fft.h"
#include "wav.h"

/* 
	Module to digitally process wav signals using a variety of filters

	Got to give credit where credit is due: thank you meoworkshop for your
	blog on delay lines, comb filters and the shroeder reverberator. If you 
	haven't, check their blog out: 

	https://www.meoworkshop.org/silly-audio-processing-6/
*/

// number of allpass and feedback comb filters in shroeder reverberator 
#define NUM_APCFS 3
#define NUM_FBCFS 4

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

void current_delay_line(delay_line *DL, float *output);
/* returns the current value being pointed to in the delay line */

void delete_delay_line(delay_line *DL);
/* free all memory, clear all buffers... you know that sort of stuff. */

void delay_line_main();
/* sends input data through a delay line of user-defined length and writes 
   the delayed data to a user-defined .wav */

/* Delay Line End ***********************************************************/

/* Feed-Forward Comb Filter *************************************************/

typedef struct ff_comb_filter {

	float b0;	// coefficient to scale input
	float bm;	// coefficient to scale delay line output
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

void ff_comb_filter_main();
/* sends input data through a feed-forward comb filter with user-defined
   coefficients and writes the filtered data to a user-defined .wav */

/* Feed-Forward Comb Filter End *********************************************/

/* Feedback Comb Filter ****************************************************/
typedef struct fb_comb_filter {

	float b0;	// coefficient to scale new delay line output
	float am;	// coefficient to scale current delay line value
	delay_line *DL;

} fb_comb_filter;

fb_comb_filter* init_fb_comb_filter(size_t delay_length, float b0, float am);
/* initialize feed-back comb filter, defining the delay length and the scalar
   consants b0 and am. */

void step_fb_comb_filter(fb_comb_filter *FBCF, float input, float *output);
/* sends the input data through the comb filter, using the delay line and 
   scalar constants, and outputs the filtered data. */

void delete_fb_comb_filter(fb_comb_filter *FBCF);
/* free all memory, clear all buffers... you know that sort of stuff. */

void fb_comb_filter_main();
/* sends input data through a feed-back comb filter with user-defined 
   coefficients and writes the filtered data to a user-defined .wav */

/* Feedback Comb Filter End *************************************************/

/* Allpass Comb Filter ******************************************************/

typedef struct ap_comb_filter {
	ff_comb_filter *FFCF;
	fb_comb_filter *FBCF;
} ap_comb_filter;

ap_comb_filter* init_ap_comb_filter(size_t delay_length, float b0, float am);
/* initialize all-pass comb filter, defining the delay length and the scalar
   consants b0 and am. */

void step_ap_comb_filter(ap_comb_filter *APCF, float input, float *output);
/* sends the input data through the comb filter, using the delay line and 
   scalar constants, and outputs the filtered data. */

void delete_ap_comb_filter(ap_comb_filter *APCF);
/* free all memory, clear all buffers... you know that sort of stuff. */

void ap_comb_filter_main();
/* sends input data through an all-pass comb filter with user-defined
   coefficients and writes the filtered data to a user-defined .wav */

/* Allpass Comb Filter End *************************************************/

/* Shroeder Reverberator ***************************************************/

typedef struct shroeder_reverberator {
	ap_comb_filter *APCF[NUM_APCFS];
	fb_comb_filter *FBCF[NUM_FBCFS];
} shroeder_reverberator;

shroeder_reverberator* init_shroeder_reverberator(size_t *ap_delay_lengths, size_t *fb_delay_lengths, float ap_gain, float *fb_gains);
/* initialize shroeder reverberator, defining the delay length and the scalar
   consants for both the all-pass and feed-back comb filters. */

void step_shroeder_reverberator(shroeder_reverberator *SR, float input, float *output);
/* sends the input data through the correct sequence of filters, using the 
   delay line and scalar constants, and outputs the filtered data. */

void delete_shroeder_reverberator(shroeder_reverberator *SR);
/* free all memory, clear all buffers... you know that sort of stuff. */

void shroeder_reverberator_main();
/* sends input data through a shroeder reverberator with user-define 
   coefficients and writes the filtered data to a user-defined .wav */

/* Shroeder Reverberator End ***********************************************/

/* Convolution Reverb ******************************************************/

void overlap_add_convolution(float* input, float* impulse, float* output, 
							 int input_length, int impulse_length);

void fft_convolution(Complex *input, Complex *impulse, Complex *output,
							  int input_length, int impulse_length, int rd_N);

int power_2_round(int num, int direction);
/* round integer up or down to the nearest power of 2 */

void convolution_reverb_main();
/* convolves input data with a user-defined impulse response and writes the
   convolved data to a user-define .wav */

/* Convolution Reverb End **************************************************/

#endif // FILTERS_H_
