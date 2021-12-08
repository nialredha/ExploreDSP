
#include <stdint.h>
#include <stdio.h>

/* Module to generate simulated wave data of varying frequencies */

typedef struct data_obj {

	float	frequency[10];		// max of ten frequencies
	int		num_frequencies;
	
	int		sample_rate;
	int		num_samples;
	
	float*	data; 

} data_obj;

void wave_gen_i(data_obj* d, int_fast32_t* data, int M);
/* Generate a superposition of up to 10 sine waves using 32 bit ints. This is 
   meant for writing to .wav files. */

void wave_gen_f(data_obj* d, float* data);
/* Generate a superposition of up to 10 sine waves using floats. This is 
   meant for generating data for FFT. */

void normalize_wave(double* value, int num_frequencies); 
/* Normalize waveforms composed of multiple sine waves to ensure wave
   amplitudes aren't out of range when writing to .wav file. */



