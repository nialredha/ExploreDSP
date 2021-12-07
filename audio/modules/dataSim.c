#include <stdlib.h>
#include <math.h>

#include "dataSim.h"

void multiple_sin(data_info* d, int_fast32_t* data, int M) {
	double time, temp;
    double dM = (double) M;

	for(int n=0; n<d->num_samples; n++) {
		time = (double)n / (double)d->sample_rate;
		temp = 0.0;

		for(int i=0; i<d->num_frequencies; i++) {
			temp += sin(2.0*M_PI*d->frequency[i]*time);
		}

		if(d->num_frequencies > 1) {
			normalize_sound(&temp, d->num_frequencies);
		}

		data[n] = (int_fast32_t)(temp*dM);
		//printf("%d\n", data[n]);

	}
	//printf("\n");
}

void normalize_sound(double* value, int num_frequencies) {
	double max = (double)num_frequencies;
	double min = (double)(-1.0 *num_frequencies);
	double range = max - min;

	*value = (*value - min) / range;
}

void float_multiple_sin(data_info* d, float* data) {
	float time;

	for(int n=0; n<d->num_samples; n++) {
		time = (float)n / (float)d->sample_rate;

		for(int i=0; i<d->num_frequencies; i++) {
			data[n] += sin(2.0*M_PI*d->frequency[i]*time);
		}
		//printf("%f\n", data[n]);

	}
	//printf("\n");
}


