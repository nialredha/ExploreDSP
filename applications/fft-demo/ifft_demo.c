/* To Build:
   gcc inverse_fft_test.c ../../modules/fft.c ../../modules/dataSim.c -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../../modules/dataSim.h"	// module to simulate wave data
#include "../../modules/fft.h"

void main()
{
	float temp;	// used for calculating amplitudes
	
	clock_t start, stop;
	double cpu_time_used;

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

	printf("\n");
	printf("1Hz Sine Wave: sampled at 8Hz for 1 second\n");
	printf("__________________________________________\n\n");

	for(int i=0; i<Data.num_samples; ++i) 
	{
		input.r[i] = Data.data[i];
		input.i[i] = 0.0;

		printf("%f\n", Data.data[i]);
	}

	printf("__________________________________________\n\n");

	start = clock();
	cfft(&input, &output, Data.num_samples);
	stop = clock();

	cpu_time_used = ((double)(stop - start)) / CLOCKS_PER_SEC;

	printf("\n");
	printf("FFT --------------------------------------------------\n");
	printf("Transformed 1Hz Sine Wave: sampled at 8Hz for 1 second\n");
	printf("______________________________________________________\n\n");

	for(int i=0; i<Data.num_samples; ++i) 
	{
		temp = output.r[i]*output.r[i] + 
			   output.i[i]*output.i[i];
		temp = sqrt(temp);

		printf("%f, %f\n", output.r[i], output.i[i]);
	}
	
	printf("\n");
	printf("Run Time: %f seconds\n", cpu_time_used);
	printf("______________________________________________________\n\n");

	start = clock();
	ifft(&output, &inverse, Data.num_samples);
	stop = clock();

	cpu_time_used = ((double)(stop - start)) / CLOCKS_PER_SEC;

	printf("\n");
	printf("IFFT -------------------------------------------------\n");
	printf("Original 1Hz Sine Wave: sampled at 8Hz for 1 second --\n");
	printf("______________________________________________________\n\n");

	for(int i=0; i<Data.num_samples; ++i) 
	{
		temp = inverse.r[i]*inverse.r[i] + 
			   inverse.i[i]*inverse.i[i];
		temp = sqrt(temp);

		printf("%f, %f\n", inverse.r[i], inverse.i[i]);
	}

	printf("\n");
	printf("Run Time: %f seconds\n", cpu_time_used);
	printf("______________________________________________________\n\n");

}


