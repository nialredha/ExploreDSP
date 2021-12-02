#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../../modules/dataSim.h"
#include "../../modules/fft.h"

int main() {

	// FFT 
	int N = 2048; //262144;	// radix-2 algorithm requires N be a power of 2
	float data[N]; 

	for(int i=0; i<N; ++i) {
		data[i] = 0.0;
	}

	clock_t start, stop;
	double cpu_time_used;

	struct data_info d;	
	d.frequency[0] = 300.0;
	d.frequency[1] = 600.0;
	d.frequency[2] = 900.0;
	d.frequency[3] = 60.0;
	d.noise_factor = 0.0;
	d.sample_rate = N;	//44100;
	d.num_frequencies = 4;
	d.num_samples = d.sample_rate*1;	/* 1 second duration */
	d.num_channels = 0;	

	//float frequency[] = 120.0;
	//wave_gen(data, N, frequency);

   	float_multiple_sin(&d, data);
/*	
	for(int i=0; i<N; i++) {
		printf("%f\n", data[i]);
	}
*/
	start = clock();
	fft(data, N);
	stop = clock();

	cpu_time_used = ((double) (stop - start)) / CLOCKS_PER_SEC;

/*
	printf("Frequency Data:\n");
	for(int i=0; i<N; i++) {
		printf("%f\n", data[i]);
	}
*/

	printf("\n");
	for(int i=0; i<N/2; i++) {
		if(data[i] > 1.0) {
			printf("Waveform Frequency = %d Hz\n", i);
		}
	}

	printf("\n");
	printf("FFT Computation Time: %f\n", cpu_time_used);
}
