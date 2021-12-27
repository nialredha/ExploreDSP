/* 
	References:
	http://www.jackcampbellsounds.com/2017/06/25/conv3.html
	https://www.youtube.com/watch?v=fYggIQTaVx4	

	To build: gcc FFTCR_DeChaka_2Ch.c ../../modules/filters.c ../../modules/wav.c ../../modules/fft.c -lm
	To run: ./a.out
*/

#include <stdlib.h> #include <math.h>
#include <time.h>

#include "../../modules/filters.h"
#include "../../modules/wav.h"
#include "../../modules/fft.h"


void find_max_int(struct wav_info* w, int* max_int);
/* Find the max integer a wav file can express - essentially figure out a wav
   file's bit-depth to determine the largest integer expressable. 
   
   For example, if the bit depth was 16 bits, the maximum integer expressable
   would be 32767 */

int power_2_round(int num, int direction);
/* round integer up or down to the nearest power of 2 */

void print_to_binary(int value);
/* Function used to help visualize how to round down / round up to the 
   nearest power of 2. 
	   
   function prints out any integer backwards in binary. This means
   the left most bit printed is the least significant bit and the right most
   bit printed is the most significant. */

void main() {

	// initialize structs for input and impulse file to store basic info
	struct wav_info input_info;
	struct wav_info impulse_info;

	// set input and impulse file paths	
    char* input_file = "data/input_signals/DeChaka_Instrumental.wav";
	char* impulse_file = "data/impulse_responses/Highly Damped Large Room.wav";

	// open both the input and impulse file
    FILE* input = fopen(input_file,"rb");
	FILE* impulse = fopen(impulse_file, "rb");
	
	// read wav info and store it in the _info structs
	read_wav_info(&input_info, input);
	read_wav_info(&impulse_info, impulse);

	// print input and impuse .wav file info
	printf("\n");
	printf("Input:\n");
	printf("-----------------------------------\n");
    print_wav_info(&input_info);
	printf("-----------------------------------\n");
	printf("\nImpulse:\n");
	printf("-----------------------------------\n");
    print_wav_info(&impulse_info);
	printf("-----------------------------------\n");
	printf("\n");

	// max_int variables store the max integer a .wav file is configured to 
	// hold. This is used to convert .wav int data to float.
	uint32_t max_input_int;
	uint32_t max_impulse_int;

	// initialize and allocate memory for input and impulse data based on the
	// number of samples found in the input / impulse _info structs.
	int16_t *input_idata;
	input_idata = (int16_t *)malloc(sizeof(int16_t) * (input_info.num_samples*input_info.num_channels));
	int16_t *impulse_idata;
	impulse_idata = (int16_t *)malloc(sizeof(int16_t) * (impulse_info.num_samples*impulse_info.num_channels));

	// determine the largest expressable integer by looking at the number of
	// bits per sample located in the input / impulse _info structs.
	find_max_int(&input_info, &max_input_int);
	find_max_int(&impulse_info, &max_impulse_int);

	// read the .wav integer data and store it in their respective 1D arrays
	// where the first channel is stored in the first half of the array and 
	// the second channel is stored in the second half (the pattern would 
	// hold true if the number of channels were larger).
	read_wav_data(&input_info, input_idata, input); 
	read_wav_data(&impulse_info, impulse_idata, impulse); 

	// now that the data is read into arrays, there is no more need for the 
	// input and impulse .wav files.
    fclose(input);
    fclose(impulse);

	size_t input_length = input_info.num_samples*input_info.num_channels;
	size_t impulse_length = impulse_info.num_samples*impulse_info.num_channels;
	size_t output_length = input_length + (impulse_length - 1);	

	// since the output_length may not be a power of 2 (number of data points
	// needs to be a power of 2 for the Radix-2 FFT algorithm), round the 
	// output length up to the nearest power of 2.
	size_t rd_N = power_2_round(output_length, 1);

	// re-compute the input length based on the rounded output length (rd_N);
	input_length = rd_N - (impulse_length - 1);

	// initialize and allocate memory for the input and impulse data expressed
	// in complex floats based on the output length rounded up to the nearest 
	// power of 2. I continuously failed to perform a fast convolution with 
	// both audio channels in one array, so I decided to store each channel's
	// data in a completely different "complex" array.
	Complex input_fdata;
	input_fdata.r = (float *)malloc(sizeof(float) * rd_N/2);
	input_fdata.i = (float *)malloc(sizeof(float) * rd_N/2);
	Complex input_fdata_2;
	input_fdata_2.r = (float *)malloc(sizeof(float) * rd_N/2);
	input_fdata_2.i = (float *)malloc(sizeof(float) * rd_N/2);
	Complex impulse_fdata;
	impulse_fdata.r = (float *)malloc(sizeof(float) * rd_N/2);
	impulse_fdata.i = (float *)malloc(sizeof(float) * rd_N/2);
	Complex impulse_fdata_2;
	impulse_fdata_2.r = (float *)malloc(sizeof(float) * rd_N/2);
	impulse_fdata_2.i = (float *)malloc(sizeof(float) * rd_N/2);

	// convert the integer data read from the input and impulse .wav into 
	// floats. Store the values in the real component array and set the 
	// imaginary component array to zero for both channels.
	for(int i=0; i<input_info.num_samples; i++) {
		input_fdata.r[i] = ((float)input_idata[i]/(float)max_input_int);
		input_fdata.i[i] = 0.0;
	}
	for(int i=0; i<input_info.num_samples; i++) {
		input_fdata_2.r[i] = ((float)input_idata[i+input_info.num_samples]/(float)max_input_int);
		input_fdata_2.i[i] = 0.0;
	}
	for(int i=0; i<impulse_info.num_samples; i++) {
		impulse_fdata.r[i] = ((float)impulse_idata[i]/(float)max_impulse_int);
		impulse_fdata.i[i] = 0.0;
	}
	for(int i=0; i<impulse_info.num_samples; i++) {
		impulse_fdata_2.r[i] = ((float)impulse_idata[i+impulse_info.num_samples]/(float)max_impulse_int);
		impulse_fdata_2.i[i] = 0.0;
	}

	// since the output_length was rounded up to the nearest power of 2, zero-
	// pad the input and impulse complex float arrays for both channels. 
	for (int i=input_length/2; i<rd_N/2; i++)
	{
		input_fdata.r[i] = 0.0;
		input_fdata.i[i] = 0.0;

		input_fdata_2.r[i] = 0.0;
		input_fdata_2.i[i] = 0.0;
	}
	for (int i=impulse_length/2; i<rd_N/2; i++)
	{
		impulse_fdata.r[i] = 0.0;
		impulse_fdata.i[i] = 0.0;

		impulse_fdata_2.r[i] = 0.0;
		impulse_fdata_2.i[i] = 0.0;
	}

	/* My attempt at fast convolution where 2-channel data was expressed in 
	   one "complex" array.

	// zero pad the two channel input and impulse responses based on rd_N
	int pad = (impulse_length - 1) / 2;
	for (int i=input_length; i<input_info.num_samples; i--)
	{
		input_fdata.r[i+pad] = input_fdata.r[i];
	}
	for (int i=rd_N - pad; i<rd_N; i++)
	{
		input_fdata.r[i] = 0.0;
		input_fdata.i[i] = 0.0;
	}
	for (int i=input_info.num_samples; i<input_info.num_samples + pad; i++)
	{
		input_fdata.r[i] = 0.0;
		input_fdata.i[i] = 0.0;
	}

	pad = (input_length - 1) / 2;
	for (int i=impulse_length; i<impulse_info.num_samples; i--)
	{
		impulse_fdata.r[i + pad] = impulse_fdata.r[i];
	}
	for (int i=rd_N - pad; i<rd_N; i++)
	{
		impulse_fdata.r[i] = 0.0;
		impulse_fdata.i[i] = 0.0;
	}
	for (int i=impulse_info.num_samples; i<impulse_info.num_samples + pad; i++)
	{
		impulse_fdata.r[i] = 0.0;
		impulse_fdata.i[i] = 0.0;
	}

	*/

	// initialize struct for output file to store basic info
	struct wav_info output_info;

	// set .wav parameters
    output_info.num_channels = 2;		
    output_info.bits_per_sample = 16;
    output_info.sample_rate = 48000; // 44100;
    output_info.num_samples = output_length / 2; 

	// initialize and allocate memory for the output data expressed in complex
	// floats based on the output length rounded up to the nearest power of 2.
	Complex output_fdata;
	output_fdata.r = (float *)malloc(sizeof(float) * rd_N/2);
	output_fdata.i = (float *)malloc(sizeof(float) * rd_N/2);
	Complex output_fdata_2;
	output_fdata_2.r = (float *)malloc(sizeof(float) * rd_N/2);
	output_fdata_2.i = (float *)malloc(sizeof(float) * rd_N/2);

	// initialize to zero
	for(int i=0; i<rd_N/2; i++) {
		output_fdata.r[i] = 0.0;
		output_fdata.i[i] = 0.0;
		
		output_fdata_2.r[i] = 0.0;
		output_fdata_2.i[i] = 0.0;
	}

	// FFT Convolution ******************************************************

	// variables used to measure computation time
	clock_t start, stop;
	double cpu_time_used;

	// perform the convolution separately for both data channels
	start = clock();
	fft_convolution(&input_fdata, &impulse_fdata, &output_fdata, 
					   input_length/2, impulse_length/2, rd_N/2);
	fft_convolution(&input_fdata_2, &impulse_fdata_2, &output_fdata_2, 
					   input_length/2, impulse_length/2, rd_N/2);
	stop = clock();

	// compute the amount of computation time to run the fast convolution and
	// print the run time.
	cpu_time_used = ((double)(stop - start)) / CLOCKS_PER_SEC;
	printf("\n");
	printf("----------------------------------------------------\n");
	printf("FFT Convolution Run Time: %f seconds\n", cpu_time_used);
	printf("----------------------------------------------------\n");

	// Finding maximum output value for both channels to "normalize" the data 
	// and prevent integer overflows
	float record = -10.0;
	float record_2 = -10.0;
	for (int i=0; i<output_info.num_samples; i++)
	{
		// printf("%f\n", output_fdata[i]);

		// if the magnitude of a data point is larger than the current record,
		// save the data point as the new record. 
		if (abs(output_fdata.r[i]) > record)
		{
			record = abs(output_fdata.r[i]);
		}
		if (abs(output_fdata_2.r[i]) > record_2)
		{
			record_2 = abs(output_fdata_2.r[i]);
		}
	}
	// printf("%f\n", record); 

	// "normalize" the data by dividing each data point by the largest data
	// point found. After doing so, no values should be greater than 1.
	for (int i=0; i<output_info.num_samples; i++)
	{
		// printf("%f\n", output_fdata[i]);
		output_fdata.r[i] /= record + 1.0;	// "normalizing" the data
		output_fdata_2.r[i] /= record_2 + 1.0;
	}

	// set output file path and print the .wav file header info
	char* output_file = "data/FFTCR_2Ch_48kHz_IMPHighlyDampedLargeRoom.wav";	// filename to write to
	printf("\n");
	printf("---------------------------------------------\n");
	printf("Preparing to write to %s:\n",output_file);
    print_wav_info(&output_info);
	printf("---------------------------------------------\n");
	printf("\n");

	// open the output file and write the header info to the .wav file
    FILE* output = fopen(output_file,"wb");
	if(!output) {
        fprintf(stderr,"Error opening %s for output!\n",output_file);
    }

    write_wav_hdr(&output_info,output);

	// compute the largest expressable integer based on the amount of bits
	// per sample.
	int max_output_int;
	find_max_int(&output_info, &max_output_int);

	float max_output_float = (float)max_output_int;
	//printf("%f\n", max_output_float);

	// write the 2-channel data to a buffer of length 2, converting the float
	// data to int before storing it in the buffer. Afterwards, write the 2-
	// channel sampel using the write_sample() function.
	int_fast16_t sample[2];
	for(int n=0; n < output_info.num_samples; n++) {
		sample[0] = (int_fast16_t)(output_fdata.r[n]*max_output_float);
		sample[1] = (int_fast16_t)(output_fdata_2.r[n]*max_output_float);
		//printf("%f\n", output_fdata[n]);
		write_sample(&output_info, output, sample);
	}

	fclose(output);
	
	// Make sure everything is okay
	FILE* fp = fopen(output_file,"rb");
    printf("Reading back info from %s header:\n",output_file);
    print_wav_info(&output_info);
	printf("-----------------------------------\n");
	printf("\n");
    fclose(fp);

	// free all memory...
	// This feels wrong - not sure of malloc best practices. 
	free(input_idata);
	free(input_fdata.r);
	free(input_fdata.i);
	free(input_fdata_2.r);
	free(input_fdata_2.i);
	free(impulse_idata);
	free(impulse_fdata.r);
	free(impulse_fdata.i);
	free(impulse_fdata_2.r);
	free(impulse_fdata_2.i);
	free(output_fdata.r);
	free(output_fdata.i);
	free(output_fdata_2.r);
	free(output_fdata_2.i);

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

int power_2_round(int num, int direction)
{
	/* round integer up or down to the nearest power of 2 */

	int original_num = num;			// save original number before bit shifts
	int bit_num = sizeof(int) * 8;	// expecting 32 bits
	int MSB = 1 << (bit_num - 1);	// MSB has a 1 in the 32nd bit and 0 everywhere else
	int count = 0;					// Counts the amount of shifts until num has a 1 in the 32nd bit
	int rd_num;
	
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
		printf("-----------------------------------\n");
		printf("%d rounded up = %d\n", original_num, rd_num);
		printf("-----------------------------------\n");
	}
	// Round down
	else if (direction == -1)
	{
		rd_num = 1 << shift;
		printf("-----------------------------------\n");
		printf("%d rounded down = %d\n", original_num, rd_num);
		printf("-----------------------------------\n");
	}

	return rd_num;
} 

void print_to_binary(int value)
{
	/* function used to help visualize how to round down / round up to the 
	   nearest power of 2. 
	   
	   function prints out any integer backwards in binary. This means, 
	   reading from left to right, the left most bit printed is the least
	   significant bit and the right most bit printed is the most significant.
	*/

	while(1)
	{
		// if LSB is 1 then print a 1
		if (value & 1)
		{
			printf("%d", 1);
		}
		// else print a 0
		else 
		{
			printf("%d", 0);
		}
		
		// bit shift the value to the right one
		value >>= 1;

		// if all bits have been checked, print a line break and return
		if (value == 0)
		{
			printf("\n");
			return;
		}
	}
}

