#include <stdlib.h>
#include <math.h>

#include "dataSim.h"

void multiple_sin(struct data_info* d, int_fast32_t* data, int M) {
	float time;
    double dM = (double) M;

	for(int n=0; n<d->num_samples; n++) {
		time = (float)n / (float)d->sample_rate;

		for(int i=0; i<d->num_frequencies; i++) {
			data[n] += (int_fast32_t)(sin(2.0*M_PI*d->frequency[i]*time)*dM);
		}
		//printf("%d\n", data[n]);

	}
	//printf("\n");
}



