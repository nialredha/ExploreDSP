#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "fft.h"

int NUMBER_OF_STAGES = 0;	// essentially log(N) where N is the total number
							// of data points

// TODO: Finalize DFT - determine best suited inputs and outputs
void dft(float* data, float* amp, int N) 
{
	float tau = (2*M_PI);	// 2Pi	
	float real = 0.0, imag = 0.0, sum_sqs = 0.0;

	float omega, time, angle, phase[N];
	
	for(int freq = 0; freq<N; ++freq) 
	{
		omega = tau*freq;

		for(int n = 0; n<N; ++n)
		{
			time = (float)n / (float)N;
			angle = omega*time;

			real += data[n] * cos(angle);
			imag += data[n] * sin(angle);
		}

		sum_sqs = real*real + imag*imag;

		amp[freq] = sqrt(sum_sqs);
		// phase[k] = atan(imag/real);

		real = 0; imag = 0;
	}

}

// TODO: write comments :(
void cdft(Complex *input, Complex *output, int N) 
{
	float tau = (2*M_PI);	
	float omega, time, angle, phase[N];
	
	for(int freq = 0; freq<N; ++freq) 
	{
		omega = tau*freq;

		for(int n = 0; n<N; ++n)
		{
			time = (float)n / (float)N;
			angle = omega*time;

			output->r[freq] += input->r[n] * cos(angle);
			output->i[freq] += input->i[n] * sin(angle);
		}
	}
}

// TODO: write comments :(
void fft(float* data, int N)
{
	NUMBER_OF_STAGES = 0;	// reset in case of multiple calls

	int rev = 0, log_N = N;

	int *order;
	order = (int *)malloc(sizeof(int)*N);

	float *sorted_data, *sorted_data_i;
	sorted_data = (float *)malloc(sizeof(float)*N);
	sorted_data_i = (float *)malloc(sizeof(float)*N);

	// Determine the amount of stages required - since the FFT splits the data
	// into even and odd groups until each group only has 2 data points in it,
	// the number of stages in the FFT is equivalent to the amount of times 
	// you can divide the number of data points before you get 1. In other
	// words, 2^NUMBER_OF_STAGES = N.
	while (log_N > 0)
	{
		log_N >>= 1;
		if (log_N != 0)
		{
			NUMBER_OF_STAGES += 1;
		}
	}

	// Initialize all arrays to zero
	for (int n=0; n<N; ++n)
	{
		order[n] = 0;
		sorted_data[n] = 0.0;
		sorted_data_i[n] = 0.0;
	}

	// Determine the new order the data is going to take on after "splitting"
	// data into even and odds. It turns out, the new index a given data point
	// takes is the bit reversal of the index it was originally in. Thus, we
	// call reverse_bits() to save the new order so we can reorganize the data. 
	for (int n=0; n<N; ++n)
	{
		rev = reverse_bits(n);
		order[n] = rev;
	}

	// Reorganize the real input data, using "order[n]" as the new index. 
	for (int n=0; n<N; ++n)
	{
		sorted_data[n] = data[order[n]];
	}

	// INTIALIZATION 
	int n = 0, inc = 1, k = 0, k_step = 0, count = 0;
	
	float const_exp = 2.0*M_PI/N;

	// Loop through each stage
	for (int s=0; s<NUMBER_OF_STAGES; ++s)
	{
		// THE MAIN BUTTERFLY LOOP
		//printf("Stage: %d\n", s);

		while (n<N)
		{
			// top + (bottom * W^k)
			// top - (bottom * W^k)
			
			float top = sorted_data[n];
			float top_i = sorted_data_i[n];
			float bottom = sorted_data[n+inc] * cos(const_exp*k) + 
						   sorted_data_i[n+inc] * sin(const_exp*k);
			float bottom_i = sorted_data_i[n+inc] * cos(const_exp*k) - 
							 sorted_data[n+inc] * sin(const_exp*k);
		
			sorted_data[n] = top + bottom;
			sorted_data[n+inc] = top - bottom; 
			sorted_data_i[n] = top_i + bottom_i;
			sorted_data_i[n+inc] = top_i - bottom_i;

			k += k_step; 
			count += 1;

			if (count == inc)
			{
				n += (inc +1);
				count = 0;
				k = 0;
			}
			else 
			{
				n += 1;
			}
		}

		inc *= 2; 
		k_step = N / (2*inc);
		n = 0;
	}

	// Compute magnitudes and overWrite input data with transformed, amplitude 
	// data.
	for (int n=0; n<N; ++n)
	{
		float temp = sorted_data[n]*sorted_data[n] + 
					  sorted_data_i[n]*sorted_data_i[n];
		temp = sqrt(temp);
		data[n] = temp;

		//printf("%f\n", temp);
	}

	free(sorted_data);
	free(sorted_data_i);
	free(order);

	/*
	// Write transformed data to CSV
	FILE *file;

	file = fopen("../data/FFT_data.csv", "w+");
	fprintf(file, "Time, Raw Data, Transformed Data\n");

	for (int n=0; n<N; ++n)
	{
		double temp = sorted_data[n]*sorted_data[n] +
					  sorted_data_i[n]*sorted_data_i[n];
		temp = sqrt(temp);

		fprintf(file, "%d, %lf, %lf\n", n, data[n], temp);
	}
	*/
}

void cfft(Complex *input, Complex *output, int N)
{
	NUMBER_OF_STAGES = 0;	// reset in case of multiple calls

	int rev = 0, log_N = N;

	int *order;
	order = (int *)malloc(sizeof(int)*N);

	float *sorted_data, *sorted_data_i;
	sorted_data = (float *)malloc(sizeof(float)*N);
	sorted_data_i = (float *)malloc(sizeof(float)*N);


	// Determine the amount of stages required - since the FFT splits the data
	// into even and odd groups until each group only has 2 data points in it,
	// the number of stages in the FFT is equivalent to the amount of times 
	// you can divide the number of data points before you get 1. In other
	// words, 2^NUMBER_OF_STAGES = N.
	while (log_N > 0)
	{
		log_N >>= 1;
		if (log_N != 0)
		{
			NUMBER_OF_STAGES += 1;
		}
	}

	// Initialize all arrays to zero
	for (int n=0; n<N; ++n)
	{
		order[n] = 0;
		sorted_data[n] = 0;
		sorted_data_i[n] = 0;
	}

	// Determine the new order the data is going to take on after "splitting"
	// data into even and odds. It turns out, the new index a given data point
	// takes is the bit reversal of the index it was originally in. Thus, we
	// call reverse_bits() to save the new order so we can reorganize the data. 
	for (int n=0; n<N; ++n)
	{
		rev = reverse_bits(n);
		order[n] = rev;
	}

	// Reorganize the complex input data into its new order, using "order[n]"
	// as the new index. 

	// printf("Sorted Data\n");
	for (int n=0; n<N; ++n)
	{
		sorted_data[n] = input->r[order[n]];
		sorted_data_i[n] = input->i[order[n]];
	}

	// INTIALIZATION 
	int n = 0, inc = 1, k = 0, k_step = 0, count = 0;
	
	float const_exp = 2.0*M_PI / (float)N;

	// Loop through each stage
	for (int s=0; s<NUMBER_OF_STAGES; ++s)
	{
		// THE MAIN BUTTERFLY LOOP
		//printf("Stage: %d\n", s);

		while (n<N)
		{
			// top + (bottom * W^k)
			// top - (bottom * W^k)

			float top = sorted_data[n];
			float top_i = sorted_data_i[n];
			float bottom = sorted_data[n+inc] * cos(const_exp*k) + 
						   sorted_data_i[n+inc] * sin(const_exp*k);
			float bottom_i = sorted_data_i[n+inc] * cos(const_exp*k) - 
							 sorted_data[n+inc] * sin(const_exp*k);
		
			sorted_data[n] = top + bottom;
			sorted_data[n+inc] = top - bottom; 
			sorted_data_i[n] = top_i + bottom_i;
			sorted_data_i[n+inc] = top_i - bottom_i;

			k += k_step; 
			count += 1;

			if (count == inc)
			{
				n += (inc + 1);
				count = 0;
				k = 0;
			}
			else 
			{
				n += 1;
			}
		}

		inc *= 2; 
		k_step = N / (2*inc);
		n = 0;
	}

	// Write transformed, complex data to output
	for (int n=0; n<N; ++n)
	{
		output->r[n] = sorted_data[n];
		output->i[n] = sorted_data_i[n];
	}

	free(sorted_data);
	free(sorted_data_i);
	free(order);

}

void ifft(Complex* input, Complex* output, int N)
{
	float inverse_N = 1 / (float)N;

	// Compute the complex conjugate of the data
	for (int i=0; i<N; ++i) 
	{
		input->i[i] *= -1.0;	
		
		//printf("%f\n", input->i[i]);
	}

	// cdft(input, output, N);

	NUMBER_OF_STAGES = 0;
	
	cfft(input, output, N);

	for (int i=0; i<N; ++i)
	{
		//printf("%f\n", output->i[i]);

		output->r[i] *= inverse_N;
		output->i[i] *=  inverse_N;
	}
}

int reverse_bits(int num)
{
	/* return bit reversed order */

	int rev = 0, count = 0, max_count = NUMBER_OF_STAGES;

	while (count < max_count)
	{
		rev <<= 1;			// bitshift once to the left
		if (num & 1 == 1)	// if LSB is 1
		{
			rev ^= 1;		// set LSB to 1
		}
		num >>= 1;			// bitshift once to the right
		count += 1;
	}

	return rev; 
} 

