/*	FFT Demo/Explanation using the radix-2 algorithm implemented by me in C.
	
	The main goal here is to provide a decent explanation of what a fourier 
	transform actually is and specifically how both the discrete and fast 
	fourier transform work in general and in the context of the code that I 
	wrote. 
	
	The secondary goal is to showcase my implementation of the FFT because
	I am proud of it.

	This is most certainly for my own sake - although I have a good 
	understanding of fourier transforms, it is not good enough (meaning
	I can't effectively explain it to someone else). So, that is what we are 
	doing here, trying to explain to understand. 

	This code is meant to be accompanied with the README file located in the
	same directory this .c file is located. The point of this .c file is to 
	give you the opportunity to engage with the algorithms and to showcase
	the work that I have done. 

	All things considered, I would like this reference to help others 
	understand fourier transforms in mathematical and in computational terms.
*/

#include <stdio.h>
#include <stdlib.h>

#include <math.h>	// Used for M_PI and for sin() and cos()
#include <time.h>

#include "../../modules/dataSim.h"	// module to simulate wave data
#include "../../modules/fft.h"		// module to compute the transform

int main() {

	// variables used for measuring the speed of the transformtion
	clock_t start, stop;
	double cpu_time_used;

	// (TODO): figure out what can be done about the buffer size limitation
	// MAX Buffer Size = 262144
	int N = 32;	// radix-2 algorithm requires N be a power of 2
	
	data_info d;	

	d.frequency[0] = 30000.0;
	d.frequency[1] = 600.0;
	d.frequency[2] = 9000.0;
	d.frequency[3] = 60.0;
	d.num_frequencies = 1;

	d.sample_rate = N;	
	d.num_samples = d.sample_rate*1;	// 1 second duration
	
	d.noise_factor = 0.0;	// option to add noise not implemented yet 
	d.num_channels = 0;		// important when working with .wav files	

	// define and initialize data buffer to 0
	float data[N]; 
	for(int i=0; i<N; ++i) {
		data[i] = 0.0;
	}

	// generate the data
   	float_multiple_sin(&d, data);

/*
	for(int i=0; i<N; i++) {
		printf("%f\n", data[i]);
	}
*/

	// transform the data and time it 
	start = clock();
	fft(data, N);
	stop = clock();

	// calculate the computation time 
	cpu_time_used = ((double) (stop - start)) / CLOCKS_PER_SEC;

	// print amplitude data in frequency domain
	printf("Frequency Data:\n");
	for(int i=0; i<N; i++) {
		printf("%f\n", data[i]);
	}

	// determine the frequencies using transformed data by checking which
	// index in the array has a significantly non-zero value (> 1.0)
	printf("\n");
	for(int i=0; i<N/2; i++) {
		if(data[i] > 1.0) {
			printf("Waveform Frequency = %d Hz\n", i);
		}
	}

	// print the computation time it took to run the FFT
	printf("\n");
	printf("FFT Computation Time: %f\n", cpu_time_used);
}
