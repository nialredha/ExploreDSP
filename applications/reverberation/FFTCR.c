/* 
	References:
	http://www.jackcampbellsounds.com/2017/06/25/conv3.html
	https://www.youtube.com/watch?v=fYggIQTaVx4	

	To build: gcc FFTCR.c ../../modules/filters.c ../../modules/wav.c ../../modules/fft.c -lm
	To run: ./a.out
*/

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../../modules/filters.h"
#include "../../modules/wav.h"
#include "../../modules/fft.h"


void find_max_int(struct wav_info* w, int* max_int);
/* Find the max integer a wav file can express - essentially figure out a wav
   file's bit-depth to determine the largest integer expressable. 
   
   For example, if the bit depth was 16 bits, the maximum integer expressable
   would be 32767 */

void print_to_binary(int value);
int power_2_round(int num, int direction);

void main() {

	struct wav_info input_info;
	struct wav_info impulse_info;

    char* input_file = "data/input_signals/DeChaka_Instrumental.wav";
	char* impulse_file = "data/impulse_responses/Going Home.wav";

    FILE* input = fopen(input_file,"rb");
	FILE* impulse = fopen(impulse_file, "rb");
	
	read_wav_info(&input_info, input);
	read_wav_info(&impulse_info, impulse);

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

	uint32_t max_input_int;				/* used to convert wav data to float */
	uint32_t max_impulse_int;			/* used to convert wav data to float */

	// printf("Sample Number: %d\n", input_info.num_samples);

	int16_t *input_idata;
	input_idata = (int16_t *)malloc(sizeof(int16_t) * (input_info.num_samples*input_info.num_channels));

	int16_t *impulse_idata;
	impulse_idata = (int16_t *)malloc(sizeof(int16_t) * (impulse_info.num_samples*impulse_info.num_channels));

	find_max_int(&input_info, &max_input_int);
	find_max_int(&impulse_info, &max_impulse_int);

	read_wav_data(&input_info, input_idata, input); 
	read_wav_data(&impulse_info, impulse_idata, impulse); 

    fclose(input);
    fclose(impulse);

	size_t input_length = input_info.num_samples;
	size_t impulse_length = impulse_info.num_samples;
	size_t output_length = input_length + impulse_length - 1;	

	size_t N = input_length + (impulse_length - 1);

	size_t rd_N = power_2_round(N, 1);
	input_length = rd_N - (impulse_length - 1);

	Complex input_fdata;
	input_fdata.r = (float *)malloc(sizeof(float) * rd_N);
	input_fdata.i = (float *)malloc(sizeof(float) * rd_N);
	// input_fdata = (float *)malloc(sizeof(float) * (input_info.num_samples*input_info.num_channels));

	Complex impulse_fdata;
	impulse_fdata.r = (float *)malloc(sizeof(float) * rd_N);
	impulse_fdata.i = (float *)malloc(sizeof(float) * rd_N);
	// impulse_fdata = (float *)malloc(sizeof(float) * (impulse_info.num_samples*impulse_info.num_channels));

	// For now only going to try on one channel 
	for(int i=0; i<input_info.num_samples; i++) {
		input_fdata.r[i] = ((float)input_idata[i]/(float)max_input_int);
		input_fdata.i[i] = 0.0;
		//printf("%f\n", input_fdata[i]);
	}
	for(int i=0; i<impulse_info.num_samples; i++) {
		impulse_fdata.r[i] = ((float)impulse_idata[i]/(float)max_impulse_int);
		impulse_fdata.i[i] = 0.0;
		//printf("%f\n", impulse_fdata[i]);
	}

	// This is for block-based convolution
	// size_t input_buffer_length = 2048;

	// zero pad the input and impulse response based rd_N
	for (int i=input_length; i<rd_N; i++)
	{
		input_fdata.r[i] = 0.0;
		input_fdata.i[i] = 0.0;
	}
	for (int i=impulse_length; i<rd_N; i++)
	{
		impulse_fdata.r[i] = 0.0;
		impulse_fdata.i[i] = 0.0;
	}

	struct wav_info output_info;

    output_info.num_channels = 1;		// again one channel for now
    output_info.bits_per_sample = 16;
    output_info.sample_rate = 44100;
    output_info.num_samples = output_length; 

	Complex output_fdata;
	output_fdata.r = (float *)malloc(sizeof(float) * rd_N);
	output_fdata.i = (float *)malloc(sizeof(float) * rd_N);

	for(int i=0; i<rd_N; i++) {
		output_fdata.r[i] = 0.0;
		output_fdata.i[i] = 0.0;
	}

	// FFT Convolution 
	clock_t start, stop;
	double cpu_time_used;

	start = clock();
	fft_convolution(&input_fdata, &impulse_fdata, &output_fdata, 
					   input_length, impulse_length, rd_N);
	stop = clock();

	
	cpu_time_used = ((double)(stop - start)) / CLOCKS_PER_SEC;

	printf("\n");
	printf("----------------------------------------------------\n");
	printf("FFT Convolution Run Time: %f seconds\n", cpu_time_used);
	printf("----------------------------------------------------\n");

	/* Finding maximum output value to "normalize" */
	float record = -10.0;
	for (int i=0; i<output_info.num_samples; i++)
	{
		// printf("%f\n", output_fdata[i]);

		if (abs(output_fdata.r[i]) > record)
		{
			record = abs(output_fdata.r[i]);
		}
	}
	// printf("%f\n", record); 

	for (int i=0; i<output_info.num_samples; i++)
	{
		// printf("%f\n", output_fdata[i]);
		output_fdata.r[i] /= record + 1.0;	// "normalizing" the data
	}

	char* output_file = "data/FFTCR_1Ch_IMPGoingHome.wav";	// filename to write to
	printf("\n");
	printf("---------------------------------------------\n");
	printf("Preparing to write to %s:\n",output_file);
    print_wav_info(&output_info);
	printf("---------------------------------------------\n");
	printf("\n");

    FILE* output = fopen(output_file,"wb");
	if(!output) {
        fprintf(stderr,"Error opening %s for output!\n",output_file);
    }

	int max_output_int;
	find_max_int(&output_info, &max_output_int);

	float max_output_float = (float)max_output_int;
	//printf("%f\n", max_output_float);

    write_wav_hdr(&output_info,output);

	for(int n=0; n < output_info.num_samples; n++) {
		int_fast16_t sample = (int_fast16_t)(output_fdata.r[n]*max_output_float);
		//printf("%f\n", output_fdata[n]);
		write_sample(&output_info, output, &sample);
	}

	fclose(output);
	
	// Make sure everything is okay
	FILE* fp = fopen(output_file,"rb");
    read_wav_info(&output_info,fp);
	printf("-----------------------------------\n");
    printf("Reading back info from %s header:\n",output_file);
    print_wav_info(&output_info);
	printf("-----------------------------------\n");
	printf("\n");
    fclose(fp);

	free(input_idata);
	free(input_fdata.r);
	free(input_fdata.i);
	free(impulse_idata);
	free(impulse_fdata.r);
	free(impulse_fdata.i);
	free(output_fdata.r);
	free(output_fdata.i);

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
	/* round integer down to the nearest power of 2 */

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

	if (direction == 1)
	{
		rd_num = 1 << shift + 1;
		printf("-----------------------------------\n");
		printf("%d rounded up = %d\n", original_num, rd_num);
		printf("-----------------------------------\n");
	}
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
		if (value & 1)
		{
			printf("%d", 1);
		}
		else 
		{
			printf("%d", 0);
		}

		value >>= 1;

		if (value == 0)
		{
			printf("\n");
			return;
		}
	}
}

