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

void delay_line_main() {
	
	// initialize struct containing input data header info and pointer to 
	// input data
	struct wav_info input;
	
	float *fdata = retrieve_data(&input);

	if(fdata == NULL) { return; }

	// intialize delay line struct and determine the delay length specified by
	// the user.
	delay_line* DL;

	printf("\n");
	printf("Enter Desired Delay Length: ");
	size_t delay_length;
	scanf("%zu", &delay_length);

	// double check the delay length was read correctly.
	printf("\n");
	printf("You entered %zu as your delay length\n", delay_length);

	DL = init_delay_line(delay_length);

	float out;	// temporary variable to store delayj line output
	float *fout = (float *)malloc(sizeof(float)*((input.num_samples+delay_length)*input.num_channels));

	if(fout == NULL) { return; }

	// step each channel's data through the delay line - e.g. if 2-channel, 
	// the first half is channel 1 and the second half is channel 2. If the
	// index is greater than the number of samples in each channel, input 0
	// into the circular buffer and pull the remaining values. After 
	// completing a channel, clear the delay line buffer and repeat the steps
	// on the next channel.
	for (int c=0; c<input.num_channels; c++)
	{
		for(int i=0; i<input.num_samples+delay_length; i++)
		{
			if(i<input.num_samples)
			{
				step_delay_line(DL, fdata[i+(input.num_samples*c)], &out);
				fout[i+((input.num_samples+delay_length)*c)] = out;
			}
			else
			{
				step_delay_line(DL, 0.0, &out);
				fout[i+((input.num_samples+delay_length)*c)] = out;
			}
		}

		// clear the buffer before the next channel
		delete_delay_line(DL);
		DL = init_delay_line(delay_length);
	}

	delete_delay_line(DL);

	// no longer need fdata now that everything is stored in fout
	free(fdata);

	// initialize struct for output file to store basic info
	struct wav_info output;

	// set output .wav parameters
    output.num_channels = input.num_channels;		
    output.bits_per_sample = input.bits_per_sample;
    output.sample_rate = input.sample_rate; 
    output.num_samples = input.num_samples + delay_length;

	printf("\n");
	printf("Enter relative file path to delayed .wav file: ");
	create_file_write_data(&output, fout);
	
	free(fout);
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

void ff_comb_filter_main() {

	// initialize struct containing input data header info and pointer to 
	// input data
	struct wav_info input;
	
	float *fdata = retrieve_data(&input);

	if(fdata == NULL) { return; }
	
	ff_comb_filter* FFCF;

	printf("\n");
	printf("Enter Desired Delay Length (10500): ");
	size_t delay_length;	// 10500
	scanf("%zu", &delay_length);

	// feed forward comb filter coefficients
	printf("\n");
	printf("Enter Desired Value For b0 (1.0): ");
	float b0;	// 1.0	
	scanf("%f", &b0);
	printf("\n");
	printf("Enter Desired Value For bm (0.9): ");
	float bm;	// 0.9
	scanf("%f", &bm);
	
	FFCF = init_ff_comb_filter(delay_length, b0, bm);

	float out = 0.0;	// temporary variable to transport the filter output
	float *fout = (float *)malloc(sizeof(float)*((input.num_samples+delay_length)*input.num_channels));

	if(fout == NULL) { return; }

	// step each channel's data through the delay line - e.g. if 2-channel, 
	// the first half is channel 1 and the second half is channel 2. If the
	// index is greater than the number of samples in each channel, input 0
	// into the circular buffer and pull the remaining values. After 
	// completing a channel, clear the delay line buffer and repeat the steps
	// on the next channel.
	for (int c=0; c<input.num_channels; c++)
	{
		for(int i=0; i<input.num_samples+delay_length; i++)
		{
			if(i<input.num_samples)
			{
				step_ff_comb_filter(FFCF, fdata[i+(input.num_samples*c)], &out);
				fout[i+((input.num_samples+delay_length)*c)] = out;
			}
			else
			{
				step_ff_comb_filter(FFCF, 0.0, &out);
				fout[i+((input.num_samples+delay_length)*c)] = out;
			}
		}

		// clear the buffer before the next channel
		delete_ff_comb_filter(FFCF);
		FFCF = init_ff_comb_filter(delay_length, b0, bm);
	}

	delete_ff_comb_filter(FFCF);
	free(fdata);

	normalize_data(fout, (input.num_samples+delay_length)*input.num_channels);

	// initialize struct for output file to store basic info
	struct wav_info output;

	// set output .wav parameters
    output.num_channels = input.num_channels;		
    output.bits_per_sample = input.bits_per_sample;
    output.sample_rate = input.sample_rate; 
    output.num_samples = input.num_samples + delay_length;

	printf("\n");
	printf("Enter relative file path to filtered .wav file: ");
	create_file_write_data(&output, fout);
	
	free(fout);
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

void fb_comb_filter_main() {

	// initialize struct containing input data header info and pointer to 
	// input data
	struct wav_info input;
	
	float *fdata = retrieve_data(&input);

	if(fdata == NULL) { return; }

	fb_comb_filter* FBCF;

	printf("\n");
	printf("Enter Desired Delay Length (4000): ");
	size_t delay_length;	// 4000
	scanf("%zu", &delay_length);

	// feed back comb filter coefficients
	printf("\n");
	printf("Enter Desired Value For b0 (0.5): ");
	float b0;	// 0.5	
	scanf("%f", &b0);
	printf("\n");
	printf("Enter Desired Value For am (-0.8): ");
	float am;	// -0.8
	scanf("%f", &am);

	FBCF = init_fb_comb_filter(delay_length, b0, am);

	float out = 0.0;	// temporary variable to transport the filter output
	float *fout = (float *)malloc(sizeof(float)*((input.num_samples+delay_length)*input.num_channels));

	if(fout == NULL) { return; }

	// step each channel's data through the delay line - e.g. if 2-channel, 
	// the first half is channel 1 and the second half is channel 2. If the
	// index is greater than the number of samples in each channel, input 0
	// into the circular buffer and pull the remaining values. After 
	// completing a channel, clear the delay line buffer and repeat the steps
	// on the next channel.
	for (int c=0; c<input.num_channels; c++)
	{
		for(int i=0; i<input.num_samples+delay_length; i++)
		{
			if(i<input.num_samples)
			{
				step_fb_comb_filter(FBCF, fdata[i+(input.num_samples*c)], &out);
				fout[i+((input.num_samples+delay_length)*c)] = out;
			}
			else
			{
				step_fb_comb_filter(FBCF, 0.0, &out);
				fout[i+((input.num_samples+delay_length)*c)] = out;
			}
		}

		// clear the buffer before the next channel
		delete_fb_comb_filter(FBCF);
		FBCF = init_fb_comb_filter(delay_length, b0, am);
	}

	delete_fb_comb_filter(FBCF);
	free(fdata);

	normalize_data(fout, (input.num_samples+delay_length)*input.num_channels);

	// initialize struct for output file to store basic info
	struct wav_info output;

	// set output .wav parameters
    output.num_channels = input.num_channels;		
    output.bits_per_sample = input.bits_per_sample;
    output.sample_rate = input.sample_rate; 
    output.num_samples = input.num_samples + delay_length;

	printf("\n");
	printf("Enter relative file path to filtered .wav file: ");
	create_file_write_data(&output, fout);
	
	free(fout);
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

void ap_comb_filter_main() {

	// initialize struct containing input data header info and pointer to 
	// input data
	struct wav_info input;
	
	float *fdata = retrieve_data(&input);

	if(fdata == NULL) { return; }

	ap_comb_filter* APCF;

	printf("\n");
	printf("Enter Desired Delay Length (4000): ");
	size_t delay_length;	// 4000
	scanf("%zu", &delay_length);

	// feed back comb filter coefficients
	printf("\n");
	printf("Enter Desired Value For b0 (0.5): ");
	float b0;	// 0.5	
	scanf("%f", &b0);
	printf("\n");
	printf("Enter Desired Value For am (-0.5): ");
	float am;	// -0.5
	scanf("%f", &am);

	APCF = init_ap_comb_filter(delay_length, b0, am);

	float out = 0.0;	// temporary variable to transport the filter output
	float *fout = (float *)malloc(sizeof(float)*((input.num_samples+delay_length)*input.num_channels));

	if(fout == NULL) { return; }

	// step each channel's data through the delay line - e.g. if 2-channel, 
	// the first half is channel 1 and the second half is channel 2. If the
	// index is greater than the number of samples in each channel, input 0
	// into the circular buffer and pull the remaining values. After 
	// completing a channel, clear the delay line buffer and repeat the steps
	// on the next channel.
	for (int c=0; c<input.num_channels; c++)
	{
		for(int i=0; i<input.num_samples+delay_length; i++)
		{
			if(i<input.num_samples)
			{
				step_ap_comb_filter(APCF, fdata[i+(input.num_samples*c)], &out);
				fout[i+((input.num_samples+delay_length)*c)] = out;
			}
			else
			{
				step_ap_comb_filter(APCF, 0.0, &out);
				fout[i+((input.num_samples+delay_length)*c)] = out;
			}
		}

		// clear the buffer before the next channel
		delete_ap_comb_filter(APCF);
		APCF = init_ap_comb_filter(delay_length, b0, am);
	}

	delete_ap_comb_filter(APCF);
	free(fdata);

	normalize_data(fout, (input.num_samples+delay_length)*input.num_channels);

	// initialize struct for output file to store basic info
	struct wav_info output;

	// set output .wav parameters
    output.num_channels = input.num_channels;		
    output.bits_per_sample = input.bits_per_sample;
    output.sample_rate = input.sample_rate; 
    output.num_samples = input.num_samples + delay_length;

	printf("\n");
	printf("Enter relative file path to filtered .wav file: ");
	create_file_write_data(&output, fout);
	
	free(fout);
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

void shroeder_reverberator_main() {

	// initialize struct containing input data header info and pointer to 
	// input data
	struct wav_info input;
	
	float *fdata = retrieve_data(&input);
	
	if(fdata == NULL) { return; }

	shroeder_reverberator* SR;

	// shroeder reverberator delay lengths and gains
	size_t ap_delay_lengths[NUM_APCFS];	// {347, 113, 37};
	printf("\n");
	printf("Enter APCF1 Delay Length (347): ");
	scanf("%zu", &ap_delay_lengths[0]);
	printf("\n");
	printf("Enter APCF2 Delay Length (113): ");
	scanf("%zu", &ap_delay_lengths[1]);
	printf("\n");
	printf("Enter APCF3 Delay Length (37): ");
	scanf("%zu", &ap_delay_lengths[2]);

	size_t fb_delay_lengths[NUM_FBCFS];	// {1687, 1601, 2053, 2251};
	printf("\n");
	printf("Enter FBCF1 Delay Length (1687): ");
	scanf("%zu", &fb_delay_lengths[0]);
	printf("\n");
	printf("Enter FBCF2 Delay Length (1601): ");
	scanf("%zu", &fb_delay_lengths[1]);
	printf("\n");
	printf("Enter FBCF3 Delay Length (2053): ");
	scanf("%zu", &fb_delay_lengths[2]);
	printf("\n");
	printf("Enter FBCF4 Delay Length (2251): ");
	scanf("%zu", &fb_delay_lengths[3]);

	float ap_gain;	// 0.7
	printf("\n");
	printf("Enter Desired Value For APCF Gain (0.7): ");
	scanf("%f", &ap_gain);

	float fb_gains[NUM_FBCFS];	// {0.773, 0.802, 0.753, 0.733};
	printf("\n");
	printf("Enter Desired Value For FBCF1 Gain (0.773): ");
	scanf("%f", &fb_gains[0]);
	printf("\n");
	printf("Enter Desired Value For FBCF2 Gain (0.802): ");
	scanf("%f", &fb_gains[1]);
	printf("\n");
	printf("Enter Desired Value For FBCF3 Gain (0.753): ");
	scanf("%f", &fb_gains[2]);
	printf("\n");
	printf("Enter Desired Value For FBCF4 Gain (0.733): ");
	scanf("%f", &fb_gains[3]);

	SR = init_shroeder_reverberator(ap_delay_lengths, fb_delay_lengths, ap_gain, fb_gains); 

	float out = 0.0;	// temporary variable to transport the filter output
	float *fout = (float *)malloc(sizeof(float)*((input.num_samples)*input.num_channels));

	if(fout == NULL) { return; }

	// step each channel's data through the delay line - e.g. if 2-channel, 
	// the first half is channel 1 and the second half is channel 2. If the
	// index is greater than the number of samples in each channel, input 0
	// into the circular buffer and pull the remaining values. After 
	// completing a channel, clear the delay line buffer and repeat the steps
	// on the next channel.
	for (int c=0; c<input.num_channels; c++)
	{
		for(int i=0; i<input.num_samples; i++)
		{
			step_shroeder_reverberator(SR, fdata[i+(input.num_samples*c)], &out);
			fout[i+((input.num_samples)*c)] = out;
		}

		// clear the buffer before the next channel
		delete_shroeder_reverberator(SR);
		SR = init_shroeder_reverberator(ap_delay_lengths, fb_delay_lengths, ap_gain, fb_gains); 
	}

	delete_shroeder_reverberator(SR);
	free(fdata);

	normalize_data(fout, (input.num_samples)*input.num_channels);

	// initialize struct for output file to store basic info
	struct wav_info output;

	// set output .wav parameters
    output.num_channels = input.num_channels;		
    output.bits_per_sample = input.bits_per_sample;
    output.sample_rate = input.sample_rate; 
    output.num_samples = input.num_samples;

	printf("\n");
	printf("Enter relative file path to filtered .wav file: ");
	create_file_write_data(&output, fout);
	
	free(fout);
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

int power_2_round(int num, int direction) {
/* round integer up or down to the nearest power of 2 */

	int original_num = num;			// save original number before bit shifts
	int bit_num = sizeof(int) * 8;	// expecting 32 bits
	int MSB = 1 << (bit_num - 1);	// MSB has a 1 in the 32nd bit and 0 everywhere else
	int count = 0;					// counts the amount of shifts until num has a 1 in the 32nd bit
	int rd_num;						// num rounded to the nearest power of 2
	
	// loop through 31 times and check if num & MSB == 1
	for (int i=0; i<(bit_num - 1); i++)
	{
		count ++;

		// printf("i = %d\n", i);
		// printf("count = %d\n", count);

		// if true, count should now reflect how far away num's most 
		// significant bit is from the 32nd bit. 
		if (num & MSB)	
		{
			break;	// we found what we were looking for
		}			

		// bit shift num 1 to the left until we find it's most significant bit
		num <<= 1;	
	}

	// calculate the amount of left bit-shifts are needed to represent num
	// rounded down to the nearest power of 2. 
	int shift = bit_num - count;	

	// Round up
	if (direction == 1)
	{
		rd_num = 1 << shift + 1;
		printf("\n");
		printf("-----------------------------------\n");
		printf("%d rounded up to nearest power of 2 = %d\n", original_num, rd_num);
		printf("-----------------------------------\n");
	}
	// Round down
	else if (direction == -1)
	{
		rd_num = 1 << shift;
		printf("\n");
		printf("-----------------------------------\n");
		printf("%d rounded down to nearest power of 2 = %d\n", original_num, rd_num);
		printf("-----------------------------------\n");
	}

	return rd_num;
}

void convolution_reverb_main() {

	// variables used to measure computation time
	clock_t start, stop;
	double cpu_time_used;

	// initialize struct containing input data header info and pointer to 
	// input data
	struct wav_info input;
	struct wav_info impulse;
	
	printf("\n");
	printf("INPUT WAV FILE:\n");
	printf("---------------");
	float *finput = retrieve_data(&input);
	printf("\n");
	printf("IMPULSE WAV FILE:\n");
	printf("---------------");
	float *fimpulse = retrieve_data(&impulse);

	if (finput == NULL || fimpulse == NULL) { return; }

	if(input.num_channels != impulse.num_channels)
	{
		printf("\n");
		printf("WARNING: input and impulse .wav files have different number of channels.\n");

		if(input.num_channels > impulse.num_channels)
		{
			printf("\n");
			printf("ERROR: impulse cannot be mono if input is stereo!\n");
			return;
		}

		// return;

		/*
		SAME_NUM_CHANNELS = 0;
		if(input.num_channels == 1)
		{
			for(int i=0; i<input.num_samples; i++) 
			{
				finput[i+input.num_samples] = finput[i];
			}
			input.num_channels = 2;

			printf("Converted input to stereo to self solve this error\n");
		}
		*/
	}

	size_t output_length = input.num_samples + (impulse.num_samples - 1);

	// since the output_length may not be a power of 2 (number of data points
	// needs to be a power of 2 for the Radix-2 FFT algorithm), round the 
	// output length up to the nearest power of 2.
	size_t rd_N = power_2_round(output_length*input.num_channels, 1);

	Complex cfinput;
	cfinput.r = (float *)malloc(sizeof(float) * rd_N/input.num_channels);
	cfinput.i = (float *)malloc(sizeof(float) * rd_N/input.num_channels);
	Complex cfimpulse;
	cfimpulse.r = (float *)malloc(sizeof(float) * rd_N/input.num_channels);
	cfimpulse.i = (float *)malloc(sizeof(float) * rd_N/input.num_channels);
	Complex cfoutput;
	cfoutput.r = (float *)malloc(sizeof(float) * rd_N/input.num_channels);
	cfoutput.i = (float *)malloc(sizeof(float) * rd_N/input.num_channels);

	if(cfinput.r   == NULL || cfinput.i   == NULL ||
	   cfimpulse.r == NULL || cfimpulse.i == NULL ||
	   cfoutput.r  == NULL || cfoutput.i  == NULL) 
	{return;}

	float *fout = (float *)malloc(sizeof(float)*(output_length*input.num_channels));

	if(fout == NULL) { return; }

	for(int c=0; c<input.num_channels; c++)
	{
		for(int i=0; i<rd_N/input.num_channels; i++)
		{
			if(i<input.num_samples) 
			{
				cfinput.r[i] = finput[i+((input.num_samples)*c)];
				cfinput.i[i] = 0.0;
			}
			else
			{
				cfinput.r[i] = 0.0;
				cfinput.i[i] = 0.0;
			}

			if(i<impulse.num_samples)
			{
				cfimpulse.r[i] = fimpulse[i+((impulse.num_samples)*c)];
				cfimpulse.i[i] = 0.0;
			}
			else
			{
				cfimpulse.r[i] = 0.0;
				cfimpulse.i[i] = 0.0;
			}
		}

		printf("\n");
		printf("Convolving Channel %d...\n", c+1);
		start = clock();
		fft_convolution(&cfinput, &cfimpulse, &cfoutput, 
						input.num_samples, impulse.num_samples, 
						rd_N/input.num_channels);
		stop = clock();
		printf("Channel %d Convoluted Successfully!\n", c+1);
		// compute the amount of computation time to run the fast convolution and
		// print the run time.
		cpu_time_used = ((double)(stop - start)) / CLOCKS_PER_SEC;
		printf("FFT Convolution Run Time: %f seconds\n", cpu_time_used);
		printf("----------------------------------------------------\n");

		for(int i=0; i<output_length; i++)
		{
			fout[i+(output_length*c)] = cfoutput.r[i];
		}
	}
	
	free(cfinput.r);
	free(cfinput.i);
	free(cfimpulse.r);
	free(cfimpulse.i);
	free(cfoutput.r);
	free(cfoutput.i);

	normalize_data(fout, output_length*input.num_channels); 

	// initialize struct for output file to store basic info
	struct wav_info output;

	// set output .wav parameters
    output.num_channels = input.num_channels;		
    output.bits_per_sample = input.bits_per_sample;
    output.sample_rate = input.sample_rate; 
    output.num_samples = output_length;

	printf("\n");
	printf("Enter relative file path to filtered .wav file: ");
	create_file_write_data(&output, fout);
	
	free(fout);
}

/* Convolution Reverb End ***************************************************/
