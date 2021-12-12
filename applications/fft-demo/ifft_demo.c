/* To Build:
   gcc inverse_fft_test.c ../../modules/fft.c ../../modules/dataSim.c -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../../modules/dataSim.h"	// module to simulate wave data
#include "../../modules/fft.h"

void main()
{
	data_obj Data;

	Data.frequency[0] = 1.0;	// 1Hz 
	Data.num_frequencies = 1;

	Data.sample_rate = 8;	
	Data.num_samples = Data.sample_rate*1;	// 1 second duration

	Data.data = (float*)(malloc(Data.num_samples * sizeof Data.data)); 

	Complex input, output, inverse;
	
	input.r = (float*)(malloc(Data.num_samples * sizeof input.r)); 
	input.i = (float*)(malloc(Data.num_samples * sizeof input.i)); 

	output.r = (float*)(malloc(Data.num_samples * sizeof output.r)); 
	output.i = (float*)(malloc(Data.num_samples * sizeof output.i)); 

	inverse.r = (float*)(malloc(Data.num_samples * sizeof inverse.r)); 
	inverse.i = (float*)(malloc(Data.num_samples * sizeof inverse.i)); 

	wave_gen_f(&Data, Data.data);

	for(int i=0; i<Data.num_samples; ++i) 
	{
		input.r[i] = Data.data[i];
		input.i[i] = 0.0;

		printf("%f\n", Data.data[i]);
	}

	printf("\n");

	cfft(&input, &output, Data.num_samples);
	
	float temp;
	for(int i=0; i<Data.num_samples; ++i) 
	{
		temp = output.r[i]*output.r[i] + 
			   output.i[i]*output.i[i];
		temp = sqrt(temp);

		printf("%f\n", temp);
	}

	printf("\n");

	ifft(&output, &inverse, Data.num_samples);

	for(int i=0; i<Data.num_samples; ++i) 
	{
		temp = inverse.r[i]*inverse.r[i] + 
			   inverse.i[i]*inverse.i[i];
		temp = sqrt(temp);

		printf("%f\n", temp);
	}

	printf("\n");
}


