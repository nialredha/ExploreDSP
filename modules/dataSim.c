#include <stdlib.h>
#include <math.h>

#include "dataSim.h"

void wave_gen_i(data_obj* d, int_fast32_t* data, int M) {
/* Generates a superposition of up to 10 sine waves using 32 bit ints. This is 
   meant for writing to .wav files. */

	double time, temp;
    double dM = (double)M;	// represents the bit-depth of the .wav file

	for(int n=0; n<d->num_samples; n++) {
		time = (double)n / (double)d->sample_rate;	// sample / (samples/sec)
		temp = 0.0;	

		for(int i=0; i<d->num_frequencies; i++) {
			temp += sin(2.0*M_PI*d->frequency[i]*time);
		}

		if(d->num_frequencies > 1) {
			// ensure the max wave amplitude is 1 before scaling based on the  
			// bit-depth (M) of the .wav file
			normalize_wave(&temp, d->num_frequencies);	
		}

		data[n] = (int_fast32_t)(temp*dM);
	}
}

void wave_gen_f(data_obj* d, float* data) {
/* Generate a superposition of up to 10 sine waves using floats. This is 
   meant for generating data for FFT. */

	double time, temp;

	for(int n=0; n<d->num_samples; n++) {
		time = (double)n / (double)d->sample_rate;	// sample / (samples/sec)
		temp = 0.0;

		for(int i=0; i<d->num_frequencies; i++) {
			temp += sin(2.0*M_PI*d->frequency[i]*time);
		}

		if(d->num_frequencies > 1) {
			// no real reason to normalize here, but it could come in handy
			normalize_wave(&temp, d->num_frequencies);
		}

		data[n] = (float)temp;
	}
}

void normalize_wave(double* value, int num_frequencies) {
/* Normalize waveforms composed of multiple sine waves to ensure wave
   amplitudes aren't out of range when writing to .wav file. */
	
	double max = (double)num_frequencies;
	double min = (double)(-1.0 *num_frequencies);
	double range = max - min;

	*value = (*value - min) / range;
}


