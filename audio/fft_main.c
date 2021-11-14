#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "radix2.h"

int main()
{
	// FFT 
	int N = 32; //262144;	// radix-2 algorithm requires N be a power of 2
	double data[N]; 

	clock_t start, stop;
	double cpu_time_used;

	wave_gen(data, N);

	start = clock();
	double_fft(data, N);
	stop = clock();

	cpu_time_used = ((double) (stop - start)) / CLOCKS_PER_SEC;

	printf("\n");
	printf("FFT Computation Time: %f\n", cpu_time_used);
}
