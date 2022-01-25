/*
	To Build: gcc audio_processor.c -o processAudio ../../modules/wav.c ../../modules/fft.c ../../modules/filters.c -lm
	To Run: ./a.out
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../../modules/wav.h"
#include "../../modules/fft.h"
#include "../../modules/filters.h"

int STATE = 0;
int SAME_NUM_CHANNELS = 1;

/* FUNCTIONS DEFINES *******************************************************/

void delay_line_main();
/* sends input data through a delay line of user-defined length and writes 
   the delayed data to a user-defined .wav */

void ff_comb_filter_main();
/* sends input data through a feed-forward comb filter with user-defined
   coefficients and writes the filtered data to a user-defined .wav */

void fb_comb_filter_main();
/* sends input data through a feed-back comb filter with user-defined 
   coefficients and writes the filtered data to a user-defined .wav */

void ap_comb_filter_main();
/* sends input data through an all-pass comb filter with user-defined
   coefficients and writes the filtered data to a user-defined .wav */

void shroeder_reverberator_main();
/* sends input data through a shroeder reverberator with user-define 
   coefficients and writes the filtered data to a user-defined .wav */

void convolution_reverb_main();
/* convolves input data with a user-defined impulse response and writes the
   convolved data to a user-define .wav */

float* retrieve_data(struct wav_info *w);
/* prompts user to input file path, reads input data using 
   "open_file_read_data", updates and prints wav_info, converts data to
   floating point using "convert_data_to_float", and returns float data. */

int16_t* open_file_read_data(struct wav_info *w);
/* open .wav file, read int data into buffer and return stored data. */

float* convert_data_to_float(struct wav_info *w, int16_t* idata);
/* convert the int data into float using the max bits per sample stored in 
   .wav file header. */

void normalize_data(float *fdata, int N);
/* finding maximum output value to "normalize" */

void create_file_write_data(struct wav_info *w, float* fdata);
/* reads output file path, prints wav_info, writes header, converts floating
   point data back to int, and writes data to .wav. */

void find_max_int(struct wav_info* w, int* max_int);
/* Find the max integer a wav file can express - essentially figure out a wav
   file's bit-depth to determine the largest integer expressable. 
   
   For example, if the bit depth was 16 bits, the maximum integer expressable
   would be 32767 */

int power_2_round(int num, int direction);
/* round integer up or down to the nearest power of 2 */

/* END OF FUNCTION DEFINES *************************************************/

void main() {

	int state = STATE;
	
	printf("\n");
	printf("Choose Effect:\n");
	
	printf("\n");
	printf("(1) Delay Line\n"
		   "(2) Feed-Forward Comb Filter\n"
		   "(3) Feed-Back Comb Filter\n"
		   "(4) All-Pass Comb Filter\n"
		   "(5) Shroeder Reverberator\n"
		   "(6) Convolution Reverb\n");

	printf("\n");
	printf("Enter number corresponding to effect: ");
	scanf("%X", &state);

	STATE = state;

	printf("\n");
	switch(state) {
		case 1:
			printf("You Chose The Delay Line\n");
			delay_line_main();
			break;
		case 2:
			printf("You Chose The Feed-Forward Comb Filter\n");
			ff_comb_filter_main();
			break;
		case 3:
			printf("You Chose The Feed-Back Comb Filter\n");
			fb_comb_filter_main();
			break;
		case 4:
			printf("You Chose The All-Pass Comb Filter\n");
			ap_comb_filter_main();
			break;
		case 5:
			printf("You Chose The Shroeder Reverberator\n");
			shroeder_reverberator_main();
			break;
		case 6:
			printf("You Chose The Convolution Reverb\n");
			convolution_reverb_main();
			break;
	}

	printf("\n");
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
    output.sample_rate = 30000; //input.sample_rate; 
    output.num_samples = input.num_samples + delay_length;

	printf("\n");
	printf("Enter relative file path to delayed .wav file: ");
	create_file_write_data(&output, fout);
	
	free(fout);
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

float* retrieve_data(struct wav_info *w) {

	int16_t *idata;

	// prompt user to enter input file path. If the file is valid, open it and
	// read the data into the idata buffer.
	printf("\n");
	printf("Enter relative file path to .wav file: ");
	idata = open_file_read_data(w);

	if(idata == NULL) { return NULL; }

	// print input file wav header info
	printf("\n");
	printf("WAV File Header Info:\n");
	printf("-----------------------------------\n");
    print_wav_info(w);
	printf("-----------------------------------\n");
	printf("\n");

	// initialize pointer to buffer containing input data of type float,
	// convert the integer data to float, and store it in the fdata buffer.
	// free idata's memory since we will only be using fdata from now on. 
	float *fdata;
	fdata = convert_data_to_float(w, idata);
	free(idata);

	if(fdata == NULL) { return NULL; }

	return fdata;
}

int16_t* open_file_read_data(struct wav_info *w) {

	// user input relative file path to .wav file
    char file_path[1000];
	scanf("%s", file_path); 

	// char* file_path = "data/input_signals/DeChaka_Instrumental.wav";

	// loop to check file exists. The loop will not break until a correct 
	// file path has been inputed.
	while(1) 
	{
		FILE* file;

		// if it exists, close the file and break out of the loop
		if(file = fopen(file_path, "r"))
		{
			fclose(file);
			printf("\n");
			printf("File exists\n");

			break;
		}
		// else, print an error and prompt user to try again
		else
		{
			printf("\n");
			printf("Error: File does not exist\n");

			printf("\n");
			printf("Enter relative file path again: ");
			scanf("%s", file_path); 
		}
	}
	
	// open and read .wav file. "rb" means read binary 
    FILE* fp = fopen(file_path,"rb");

	// reads the header and inputs info into the wav_info struct
	read_wav_info(w, fp);

	// allocate data based on the number of samples and number of channels and
	// store the int data in the idata buffer. 
	int16_t *idata = (int16_t *)malloc(sizeof(int16_t)*(w->num_samples*w->num_channels));
	if(idata == NULL) { return NULL; }

	read_wav_data(w, idata, fp); 

	// now that the header and the data has been read and stored locally there
	// is no more need for the file. 
	fclose(fp);

	return idata;
}

float* convert_data_to_float(struct wav_info *w, int16_t* idata) {
/* find the largest expressable integer based on the .wav formatting, 
   specifically the amount of bits per sample */

	uint32_t max_int;
	find_max_int(w, &max_int);
	
	// allocate memory for float data buffer
	float *fdata = (float *)malloc(sizeof(float)*(w->num_samples*w->num_channels));

	if(fdata == NULL) { return NULL; }

	/*
	float *fdata;
	if(STATE == 6 && SAME_NUM_CHANNELS == 0 && w->num_channels == 1)
	{
		fdata = (float *)malloc(sizeof(float)*(w->num_samples*2));
	}
	else
	{
		fdata = (float *)malloc(sizeof(float)*(w->num_samples*w->num_channels));
	}
	*/

	// divide each data point by max_int to convert the data into floats
	// ranging from -1 to 1.
	for(int i=0; i<w->num_samples*w->num_channels; i++) {

		fdata[i] = ((float)idata[i]/(float)max_int);
	}

	return fdata;
}

void normalize_data(float *fdata, int N) {
/* Finding maximum output value to "normalize" */

	float record = -1000.0;
	for (int i=0; i<N; i++)
	{
		// printf("%f\n", output_fdata[i]);

		if (abs(fdata[i]) > record)
		{
			record = abs(fdata[i]);
		}
	}
	// printf("%f\n", record); 

	for (int i=0; i<N; i++)
	{
		// printf("%f\n", output_fdata[i]);
		fdata[i] /= record + 1.0;	// "normalizing" the data
	}
}

void create_file_write_data(struct wav_info *w, float *fdata) {

	// user input relative file path to .wav file
    char file_path[1000];
	scanf("%s", file_path); 

	// set output file path and print the .wav file header info
	printf("\n");
	printf("---------------------------------------------\n");
	printf("Preparing to write to %s:\n", file_path);
    print_wav_info(w);
	printf("---------------------------------------------\n");
	printf("\n");

	// open the output file and write the header info to the .wav file
    FILE* fp = fopen(file_path,"wb");
	if(!fp) {
        fprintf(stderr,"Error opening %s for output!\n",fp);
    }

    write_wav_hdr(w,fp);

	// compute the largest expressable integer based on the amount of bits
	// per sample.
	int max_int;
	find_max_int(w, &max_int);

	float max_float = (float)max_int;
	//printf("%f\n", max_output_float);

	// write the 2-channel data to a buffer of length 2, converting the float
	// data to int before storing it in the buffer. Afterwards, write the 2-
	// channel sampel using the write_sample() function.
	int_fast16_t sample[2];
	for(int i=0; i < w->num_samples; i++) 
	{
		for(int j=0; j < w->num_channels; j++)
		{
			sample[j] = (int_fast16_t)(fdata[i+(w->num_samples*j)]*max_float);
		}
		write_sample(w, fp, sample);
	}

	fclose(fp);
}


void find_max_int(struct wav_info* w, int* max_int) {
/* determine maximum integer expressable in w.bits_per_sample bits, assuming
twos complement encoding of signed integers */

    switch(w->bits_per_sample) {
    case 8:
      *max_int=0x7f;		/* 127 */
      break;
    case 16:
      *max_int=0x7fff;		/* 32767 */
      break;
    case 24:
      *max_int=0x7fffff;	/* 8388607 */
      break;
    case 32:
      *max_int=0x7fffffff;	/* 2147483647 */
      break;
    default:
      printf("error: bits_per_sample must be 8,16,24, or 32.\n");
    }
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


