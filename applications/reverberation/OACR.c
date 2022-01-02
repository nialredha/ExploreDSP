/* 
	To build: gcc OACR.c ../../modules/filters.c ../../modules/wav.c ../../modules/fft.c -lm 
	To run: ./a.out

	WARNING: This script runs an overlap-add convolution on an over 3 million
	point input signal. It will take a while to run - nothing too crazy, but
	don't expect anything remotely fast. This script is meant to show how 
	expensive this algorithm, explaining why we need the FFT. 

*/

#include <stdlib.h>
#include <time.h>

#include "../../modules/filters.h"
#include "../../modules/wav.h"

void find_max_int(struct wav_info* w, int* max_int);
/* Find the max integer a wav file can express - essentially figure out a wav
   file's bit-depth to determine the largest integer expressable. 
   
   For example, if the bit depth was 16 bits, the maximum integer expressable
   would be 32767 */

void main() {

	struct wav_info input_info;
	struct wav_info impulse_info;

    char* input_file = "data/input_signals/DeChaka_Instrumental.wav";
	char* impulse_file = "data/impulse_responses/Masonic_Lodge.wav";

    FILE* input = fopen(input_file,"rb");
	FILE* impulse = fopen(impulse_file, "rb");
	
	read_wav_info(&input_info, input);
	read_wav_info(&impulse_info, impulse);

	printf("Input:\n");
    print_wav_info(&input_info);
	printf("\nImpulse:\n");
    print_wav_info(&impulse_info);
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


	float *input_fdata;
	input_fdata = (float *)malloc(sizeof(float) * (input_info.num_samples*input_info.num_channels));

	float *impulse_fdata;
	impulse_fdata = (float *)malloc(sizeof(float) * (impulse_info.num_samples*impulse_info.num_channels));

	// For now only going to try on one channel 
	for(int i=0; i<input_info.num_samples; i++) {
		input_fdata[i] = ((float)input_idata[i]/(float)max_input_int);
		//printf("%f\n", input_fdata[i]);
	}
	for(int i=0; i<impulse_info.num_samples; i++) {
		impulse_fdata[i] = ((float)impulse_idata[i]/(float)max_impulse_int);
		//printf("%f\n", impulse_fdata[i]);
	}

	//printf("max int = %d \n", max_int);

	size_t input_length = input_info.num_samples;
	size_t impulse_length = impulse_info.num_samples;

	printf("input_length: %d\n", input_length);
	printf("impulse_length: %d\n\n", impulse_length);

	size_t output_length = input_length + impulse_length - 1;	

	struct wav_info output_info;

    output_info.num_channels = 1;		// again one channel for now
    output_info.bits_per_sample = 16;
    output_info.sample_rate = 44100;
    output_info.num_samples = output_length; 

	// float output_fdata[output_length];
	float *output_fdata;
	output_fdata = (float *)malloc(sizeof(float)*(output_info.num_samples*output_info.num_channels));

	for(int i=0; i<output_info.num_samples; i++) {
		output_fdata[i] = 0.0;
	}
		

	// Overlap Add Convolution 
	clock_t start, stop;
	double cpu_time_used;

	start = clock();
	// This is way too expensive, which is why the FFT needs to be used. 
	overlap_add_convolution(input_fdata, impulse_fdata, output_fdata, 
					   input_length, impulse_length);
	stop = clock();

	cpu_time_used = ((double)(stop - start)) / CLOCKS_PER_SEC;

	printf("\n");
	printf("----------------------------------------------------\n");
	printf("Overlap Add Convolution Run Time: %f seconds\n", cpu_time_used);
	printf("----------------------------------------------------\n");

	/* Finding maximum output value to "normalize" */

	float record = -10.0;
	for (int i=0; i<output_info.num_samples; i++)
	{
		// printf("%f\n", output_fdata[i]);

		if (abs(output_fdata[i]) > record)
		{
			record = abs(output_fdata[i]);
		}
	}
	// printf("%f\n", record); 

	for (int i=0; i<output_info.num_samples; i++)
	{
		// printf("%f\n", output_fdata[i]);
		output_fdata[i] /= record + 1.0;	// "normalizing" the data
	}

	char* output_file = "data/OACR_Full.wav";	// filename to write to
	printf("Preparing to write to %s:\n",output_file);
    print_wav_info(&output_info);
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
		int_fast16_t sample = (int_fast16_t)(output_fdata[n]*max_output_float);
		//printf("%f\n", output_fdata[n]);
		write_sample(&output_info, output, &sample);
	}

	fclose(output);
	
	// Make sure everything is okay
	FILE* fp = fopen(output_file,"rb");
    read_wav_info(&output_info,fp);
    printf("\nReading back info from %s header:\n",output_file);
    print_wav_info(&output_info);
    fclose(fp);

	/*
	for (int k=0; k<output_length; k++) {
		printf("%f \n", output[k]);
	}
	*/
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


