#include <stdint.h>
#include <stdio.h>

/* Module to generate simulated wav data with or without noise and of varying 
   frequencies.
*/

struct data_info {
	// need to figure out a way to create multi-frequency waves 
	float	frequency[10];	// max of ten frequencies
	float	noise_factor;
	int		sample_rate;
	int		num_frequencies;
	int		num_samples;
	int		num_channels;	// not sure how I am going to incorporate this yet
};

void single_sin(struct data_info* d, int* data);
/* Generate a simple sine wave of a given frequency and sample size at a fixed
   sample rate (> x2 nyquist sample rate). */  

void multiple_sin(struct data_info* d, int_fast32_t* data, int M);
/* Generate a two frequency beat response */ 



