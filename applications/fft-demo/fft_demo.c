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
	same directory this .c file is located in . The point of this .c file is  
	to give you the opportunity to engage with the algorithms and to showcase
	the work that I have done. 

	All things considered, I would like this reference to help others 
	understand fourier transforms in mathematical and in computational terms.

	To build and run: 
	gcc fft_demo.c ../../modules/dataSim.c ../../modules/fft.c -lm && clear && ./a.out
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

	// Let's start with the Discrete Fourier Transform

	// First let's simulate a 1Hz sine wave with 8 points. To do this, we
	// need to define the characteristics of our data using the dataSim 
	// module I wrote:

	data_obj Data;

	Data.frequency[0] = 1.0;	// 1Hz 
	Data.num_frequencies = 1;

	Data.sample_rate = 8;	
	Data.num_samples = Data.sample_rate*1;	// 1 second duration

	Data.data = (float*)(malloc(Data.num_samples * sizeof Data.data)); 

	wave_gen_f(&Data, Data.data);

	printf("\n");
	printf("1Hz Sine Wave: sampled at 8Hz for 1 second\n");
	printf("__________________________________________\n\n");
	for(int i=0; i<Data.num_samples; i++) {
		printf("%d: %f\n", i, Data.data[i]);
	}
	printf("__________________________________________\n\n");


	// Now that we have simulated the data, we can go ahead and compute the
	// discrete fourier transform using our fft.h module

	float data_dft[Data.num_samples];

	start = clock();
	dft(Data.data, data_dft, Data.num_samples);
	stop = clock();

	cpu_time_used = ((double) (stop - start)) / CLOCKS_PER_SEC;

	printf("\n");
	printf("DFT --------------------------------------------------\n");
	printf("Transformed 1Hz Sine Wave: sampled at 8Hz for 1 second\n");
	printf("______________________________________________________\n\n");
	for(int i=0; i<Data.num_samples; i++) {
		printf("%d: %f\n", i, data_dft[i]);
	}
	printf("\n");
	printf("Run Time: %f seconds\n", cpu_time_used);
	printf("______________________________________________________\n\n");

	// As you can see, the transformed data has two peaks: one at index 1 of 
	// the array and one at index 7 of the array. Now, you might be asking...
	// How does this work? What do these peaks represent? Why are there two 
	// peaks? Let's talk about it.  

	// The main idea used to transform time-domain data into the frequency-
	// domain is to walk the signal around a circle at different 
	// frequencies. 
	
	// For example, when computing the transformed value for the first index,
	// we walk the signal around a circle at a frequency of 0Hz, meaning we 
	// don't walk the signal at all! Instead, we simply add up all of the wave
	// amplitudes at each point. So, why does that give us 0? This is because
	// the input signal is a pure sine wave, which oscillates at a frequency
	// greater than 0Hz. Thus, the amplitudes of the sine wave sum to 0 when
	// walking the signal around a circle at a frequency of 0.
	
	// Alright, now let's look at the next index. Here we walk the signal 
	// around a signal at a frequency of 1Hz (hmm interesting we are walking
	// the signal around a circle at the same frequency of the signal itself).

	start = clock();
	fft(Data.data, Data.num_samples);
	stop = clock();
	
	cpu_time_used = ((double) (stop - start)) / CLOCKS_PER_SEC;

	printf("\n");
	printf("FFT --------------------------------------------------\n");
	printf("Transformed 1Hz Sine Wave: sampled at 8Hz for 1 second\n");
	printf("______________________________________________________\n\n");
	for(int i=0; i<Data.num_samples; i++) {
		printf("%d: %f\n", i, Data.data[i]);
	}
	printf("Run Time: %f seconds\n", cpu_time_used);
	printf("______________________________________________________\n\n");
}
	