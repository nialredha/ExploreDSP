#include <stdlib.h>

#include "filters.h"

/* Delay Line ***************************************************************/

delay_line* init_delay_line(size_t delay_length) {
/* initialize delay_line struct, defining the delay_length, buffer length, 
   and the setting the count to 0. */

	// Why can't I create a local variable and send its address? I know it 
	// sounds wrong, but I don't actually know what is so bad about it.
	// Essentially, delay_line DL; return &DL; instead of what's below. 
	delay_line *DL = (delay_line*)malloc(sizeof(delay_line));

	DL->buffer = (float*)malloc(sizeof(float)*delay_length);	
	
	// This is something I am not entirely familiar with, but it seems like a 
	// best practice. 
	if(DL->buffer == NULL)
	{
		free(DL->buffer);
		return NULL;
	}

	// initialize the buffer to zero
	for(int i=0; i<delay_length; ++i)
	{
		DL->buffer[i] = 0.0;
	}

	DL->count = 0;	

	DL->delay_length = delay_length;

	return DL;
}

void step_delay_line(delay_line *DL, float input, float *output) {
/* sends the input data through the delay line (operating more or less like 
   a circular buffer) and outputs the appropriate data based on the 
   delay_length. If delay_length = 3, the output will equal 0 for the first 
   three iterations. */

	if (DL->delay_length != 0)
	{
		// First, set the output to where count is pointing to (initially zero)
		*output = DL->buffer[DL->count];

		// Next, update the locations value with the input 
		DL->buffer[DL->count] = input;

		// Increment the count the next position in the buffer, 
		// If the count is at the end of the buffer, reset to the beginning (0)
		// This is essentially a circular buffer;
		DL->count += 1; 

		if(DL->count == DL->delay_length)
		{
			DL->count = 0;
		}
	}

	else 
	{
		*output = input;
	}

}

void current_delay_line(delay_line *DL, float *output) {
/* returns the current value being pointed to in the delay line */

	// TODO: need to implement some error checking
	if (DL == NULL) { return; }	
	if (DL->count <= DL->delay_length)
	{
		*output = DL->buffer[DL->count];
	}
}
void delete_delay_line(delay_line *DL) {
/* free all memory, clear all buffers... you know that sort of stuff. */

	if (DL == NULL) { return; }

	if (DL->buffer != NULL)
	{
		free(DL->buffer);
		DL->buffer = NULL;
	}

	free(DL);
	DL = NULL;

	return;
}

/* Delay Line End ***********************************************************/

/* Feed-Forward Comb Filter *************************************************/

ff_comb_filter* init_ff_comb_filter(size_t delay_length, float b0, float bm) {
/* initialize feed-forward comb filter, defining the delay length and the 
   scalar consants b0 and bm. */

	ff_comb_filter *FFCF = (ff_comb_filter*)malloc(sizeof(ff_comb_filter));

	if (FFCF == NULL) 
	{
		return NULL;
	}

	FFCF->b0 = b0;
	FFCF->bm = bm;
	
	FFCF->DL = init_delay_line(delay_length);
	if (FFCF->DL == NULL)
	{
		free(FFCF);
		return NULL;
	}

	return FFCF;

}

void step_ff_comb_filter(ff_comb_filter *FFCF, float input, float *output) {
/* sends the input data through the comb filter, using the delay line and 
   scalar constants, and outputs the filtered data. */

	float delay_line_output = 0.0;
	
	step_delay_line(FFCF->DL, input, &delay_line_output);

	*output = (delay_line_output * FFCF->bm) + (input * FFCF->b0);
}

void delete_ff_comb_filter(ff_comb_filter *FFCF) { 
/* free all memory, clear all buffers... you know that sort of stuff. */

	if (FFCF == NULL) { return; }

	delete_delay_line(FFCF->DL);

	free(FFCF);
	FFCF = NULL; 

	return;
}

/* Feed-Forward Comb Filter End *********************************************/

/* Feedback Comb Filter *************************************************/

fb_comb_filter* init_fb_comb_filter(size_t delay_length, float b0, float am) {
/* initialize feed-back comb filter, defining the delay length and the scalar
   consants b0 and am. */

	fb_comb_filter *FBCF = (fb_comb_filter*)malloc(sizeof(fb_comb_filter));

	if (FBCF == NULL) 
	{
		return NULL;
	}

	FBCF->b0 = b0;
	FBCF->am = am;
	
	FBCF->DL = init_delay_line(delay_length);
	if (FBCF->DL == NULL)
	{
		free(FBCF);
		return NULL;
	}

	return FBCF;

}

void step_fb_comb_filter(fb_comb_filter *FBCF, float input, float *output) {
/* sends the input data through the comb filter, using the delay line and 
   scalar constants, and outputs the filtered data. */

	float delay_line_output = 0.0;
	
	current_delay_line(FBCF->DL, &delay_line_output);

	*output = (delay_line_output * FBCF->am) + input;

	step_delay_line(FBCF->DL, *output, &delay_line_output);

	*output = *output * FBCF->b0;
}

void delete_fb_comb_filter(fb_comb_filter *FBCF) {
/* free all memory, clear all buffers... you know that sort of stuff. */

	if (FBCF == NULL) { return; }

	delete_delay_line(FBCF->DL);

	free(FBCF);
	FBCF = NULL; 

	return;
}

/* Feedback Comb Filter End *************************************************/

/* Allpass Comb Filter *****************************************************/

ap_comb_filter* init_ap_comb_filter(size_t delay_length, float b0, float am) {
/* initialize all-pass comb filter, defining the delay length and the scalar
   consants b0 and am. */

	ap_comb_filter *APCF = (ap_comb_filter*)malloc(sizeof(ap_comb_filter));

	if (APCF == NULL) 
	{
		return NULL;
	}

	APCF->FFCF = init_ff_comb_filter(delay_length, b0, 1.0);
	if (APCF->FFCF == NULL) 
	{
		free(APCF);
		return NULL;
	}

	APCF->FBCF = init_fb_comb_filter(delay_length, 1.0, am);
	if (APCF->FBCF == NULL)
	{
		free(APCF);
		return NULL;
	}
	
	return APCF;

}

void step_ap_comb_filter(ap_comb_filter *APCF, float input, float *output) {
/* sends the input data through the comb filter, using the delay line and 
   scalar constants, and outputs the filtered data. */
	
	float output_2 = 0.0;

	step_ff_comb_filter(APCF->FFCF, input, &output_2);
	step_fb_comb_filter(APCF->FBCF, output_2, output);

	/* I was being stupid here and am leaving it as a reminder.

	float delay_line_output = 0.0;
	float delay_line_output_2 = 0.0;

	step_delay_line(APCF->FFCF->DL, input, &delay_line_output);

	*output = (delay_line_output * APCF->FFCF->bm) + (input * APCF->FFCF->b0);

	
	current_delay_line(APCF->FBCF->DL, &delay_line_output_2);

	*output = (delay_line_output_2 * APCF->FBCF->am) + *output;

	step_delay_line(APCF->FBCF->DL, *output, &delay_line_output_2);

	*output = *output * APCF->FBCF->b0;

	*/
}

void delete_ap_comb_filter(ap_comb_filter *APCF) {
/* free all memory, clear all buffers... you know that sort of stuff. */

	if (APCF == NULL) { return; }

	delete_ff_comb_filter(APCF->FFCF);
	delete_fb_comb_filter(APCF->FBCF);

	free(APCF);
	APCF = NULL; 

	return;
}

/* Allpass Comb Filter End **************************************************/

/* Shroeder Reverberator ***************************************************/

shroeder_reverberator* init_shroeder_reverberator(size_t *ap_delay_lengths, size_t *fb_delay_lengths, float ap_gain, float *fb_gains) {
/* initialize shroeder reverberator, defining the delay length and the scalar
   consants for both the all-pass and feed-back comb filters. */

	shroeder_reverberator *SR = (shroeder_reverberator*)malloc(sizeof(shroeder_reverberator));
	if (SR == NULL) 
	{
		return NULL;
	}

	for (int i=0; i<NUM_APCFS; i++)
	{
		SR->APCF[i] = init_ap_comb_filter(ap_delay_lengths[i], -ap_gain, ap_gain); 

		if (SR->APCF[i] == NULL)
		{
			free(SR);
			return NULL;
		}

	}
	for (int i=0; i<NUM_FBCFS; i++)
	{
		SR->FBCF[i] = init_fb_comb_filter(fb_delay_lengths[i], 1.0, -fb_gains[i]);

		if (SR->FBCF[i] == NULL)
		{
			free(SR);
			return NULL;
		}
	}

	return SR;
	
}

void step_shroeder_reverberator(shroeder_reverberator *SR, float input, float *output) {
/* sends the input data through the correct sequence of filters, using the 
   delay line and scalar constants, and outputs the filtered data. */

	float sum = 0.0;

	for (int i=0; i<NUM_APCFS; i++)
	{
		step_ap_comb_filter(SR->APCF[i], input, &input);
	}

	for (int i=0; i<NUM_FBCFS; i++)
	{
		// *output = 0.0;	// just in case, but shouldn't be needed
		step_fb_comb_filter(SR->FBCF[i], input, output);

		sum += *output;
	}

	*output = sum;

	// TODO: No clue why I do this besides that I saw someone else do it...
	return;
}

void delete_shroeder_reverberator(shroeder_reverberator *SR) {
/* free all memory, clear all buffers... you know that sort of stuff. */

	for (int i=0; i<NUM_APCFS; i++)
	{
		delete_ap_comb_filter(SR->APCF[i]);
	}

	for (int i=0; i<NUM_FBCFS; i++)
	{
		delete_fb_comb_filter(SR->FBCF[i]);
	}

	free(SR);
	SR = NULL;

}

/* Shroeder Reverberator End ***********************************************/

/* Convolution Reverb ******************************************************/

void overlap_add_convolution(float* input, float* impulse, float* output, 
							 int input_length, int impulse_length) {

	for (int i=0; i<input_length; i++) {
		for (int j=0; j<impulse_length; j++) {
			output[i+j] += input[i] * impulse[j];
		}
		//printf("%f\n", output[i]);
	}
}

void fft_convolution(Complex *input, Complex *impulse, Complex *output,
							  int input_length, int impulse_length, int rd_N)
{
	cfft(input, input, rd_N);
	cfft(impulse, impulse, rd_N);

	complex_multiply(input, impulse, output, rd_N);
	ifft(output, output, rd_N);

	return;
}

/* Convolution Reverb End ***************************************************/

/* General Tools ************************************************************/
/*
void complex_multiply(Complex *a, Complex *b, Complex *o, int length)
{
	for (int i=0; i<length; i++)
	{
		o->r[i] = (a->r[i]*b->r[i]) - (a->i[i]*b->i[i]);
		o->i[i] = (a->r[i]*b->i[i]) + (a->i[i]*b->r[i]);
	}		
}
*/

/* General Tools End *******************************************************/
