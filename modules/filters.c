#include <stdlib.h>
#include "filters.h"

/* Delay Line ***************************************************************/

/* 
	Got to give credit where credit is due: thank you meoworkshop for your
	blog on delay lines. If you haven't, check his blog out: 

	https://www.meoworkshop.org/silly-audio-processing-6/
*/

delay_line* init_delay_line(size_t delay_length)
{
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

void step_delay_line(delay_line *DL, float input, float *output)
{
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

void delete_delay_line(delay_line *DL)
{
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

/* 
	Got to give credit where credit is due: thank you meoworkshop for your
	blog on comb filters. If you haven't, check his blog out: 

	https://www.meoworkshop.org/silly-audio-processing-6/
*/

ff_comb_filter* init_ff_comb_filter(size_t delay_length, float b0, float bm)
{
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

void step_ff_comb_filter(ff_comb_filter *FFCF, float input, float *output)
{
	float delay_line_output = 0.0;
	
	step_delay_line(FFCF->DL, input, &delay_line_output);

	*output = (delay_line_output * FFCF->bm) + (input * FFCF->b0);
}

void delete_ff_comb_filter(ff_comb_filter *FFCF)
{
	if (FFCF == NULL) { return; }

	delete_delay_line(FFCF->DL);

	free(FFCF);
	FFCF = NULL; 

	return;
}

/* Feed-Forward Comb Filter End *********************************************/

void convolution_reverb(float* input, float* impulse, float* output, 
						int input_length, int impulse_length) {

	for (int i=0; i<input_length; i++) {
		for (int j=0; j<impulse_length; j++) {
			output[i+j] += input[i] * impulse[j];
		}
		//printf("%f\n", output[i]);
	}

}
