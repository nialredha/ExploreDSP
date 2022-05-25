/* 
	Testing FFT Precompilation in scenarios where there are multiple FFT calls
	of the same data size.

	To build and run: 
	gcc many_ffts.c ../../modules/dataSim.c ../../modules/fft.c -lm && clear && ./a.out
*/

#include <stdio.h>
#include <stdlib.h>

#include <math.h>					// Used for M_PI and for sin() and cos()
#include <time.h>

#include "../../modules/dataSim.h"	// module to simulate wave data
#include "../../modules/fft.h"		// module to compute the transform

int main() {
	
	clock_t start, stop;
	double cpu_time_used;

	int duration = 1; // seconds
	float frequency_resolution, frequency_resolution_2;

	// Let's start with the Discrete Fourier Transform

	// First let's simulate a 1Hz sine wave with 8 points. To do this, we
	// need to define the characteristics of our data using the dataSim 
	// module I wrote:

	data_obj Data;

	Data.frequency[0] = 1.0;	// 1Hz 
	Data.num_frequencies = 1;

	Data.sample_rate = 8; // 33554432; 16777216; 8388608; 4194304; 2097152;
	Data.num_samples = Data.sample_rate*duration;	

	frequency_resolution = (float)Data.sample_rate / (float)Data.num_samples;
	Data.data = (float*)malloc(sizeof(float)*Data.num_samples);

	wave_gen_f(&Data, Data.data);

	printf("\n");
	printf("%GHz Sine Wave: sampled at %dHz for %d second(s)\n", Data.frequency[0], Data.sample_rate, duration);
	printf("______________________________________________\n\n");
	for(int i=0; i<Data.num_samples; i++) {
		printf("%d: %f\n", i, Data.data[i]);
	}
	printf("______________________________________________\n\n");

	data_obj Data_2;

	Data_2.frequency[0] = 2.0;	// 1Hz 
	Data_2.num_frequencies = 1;

	Data_2.sample_rate = 8; // 33554432; 16777216; 8388608; 4194304; 2097152;
	Data_2.num_samples = Data_2.sample_rate*duration;	

	frequency_resolution_2 = (float)Data_2.sample_rate / (float)Data_2.num_samples;
	Data_2.data = (float*)malloc(sizeof(float)*Data_2.num_samples);

	wave_gen_f(&Data_2, Data_2.data);

	printf("\n");
	printf("%GHz Sine Wave: sampled at %dHz for %d second(s)\n", Data_2.frequency[0], Data_2.sample_rate, duration);
	printf("______________________________________________\n\n");
	for(int i=0; i<Data_2.num_samples; i++) {
		printf("%d: %f\n", i, Data_2.data[i]);
	}
	printf("______________________________________________\n\n");

	// Now onto the FFT, specifically the Radix-2 Algorithm. 

	// FIRST FFT
	start = clock();
	fft(Data.data, Data.num_samples);
	stop = clock();
	
	cpu_time_used = ((double)(stop - start)) / CLOCKS_PER_SEC;

	printf("\n");
	printf("FFT of %d Point Sine Wave: \n", Data.num_samples);
	printf("Frequency Resolution = %d/%d = %G\n", Data.sample_rate, Data.num_samples, frequency_resolution);
	printf("______________________________________________________\n\n");
	for(int i=0; i<Data.num_samples; i++) {
		printf("%d: %f\n", i, Data.data[i]);
	}
	printf("\n");
	printf("Run Time: %f seconds\n", cpu_time_used);
	printf("______________________________________________________\n\n");

	// printf("number of calls = %d\n", NUMBER_OF_CALLS);
	// printf("previous call size = %d\n", PREVIOUS_CALL_SIZE);

	// SECOND FFT - same size as the last 
	start = clock();
	fft(Data_2.data, Data_2.num_samples);
	stop = clock();
	
	cpu_time_used = ((double)(stop - start)) / CLOCKS_PER_SEC;

	printf("\n");
	printf("FFT of %d Point Sine Wave: \n", Data_2.num_samples);
	printf("Frequency Resolution = %d/%d = %G\n", Data_2.sample_rate, Data_2.num_samples, frequency_resolution_2);
	printf("______________________________________________________\n\n");
	for(int i=0; i<Data_2.num_samples; i++) {
		printf("%d: %f\n", i, Data_2.data[i]);
	}
	printf("\n");
	printf("Run Time: %f seconds\n", cpu_time_used);
	printf("______________________________________________________\n\n");

	free(Data.data);
	free(Data_2.data);
}
	
