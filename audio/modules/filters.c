#include <stdlib.h>
#include "filters.h"

void convolution_reverb(float* input, float* impulse, float* output, 
						int input_length, int impulse_length) {

	for (int i=0; i<input_length; i++) {
		for (int j=0; j<impulse_length; j++) {
			output[i+j] += input[i] * impulse[j];
		}
	}

}
