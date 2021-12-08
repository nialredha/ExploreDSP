#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "fft.h"

int NUMBER_OF_STAGES = 0;

void dft(float* data, float* amp, int N) { 

	float tau = (2*M_PI)/N;	// part of the "angle" that remains constant
	float real = 0, imag = 0, sum_sqs = 0;

	float angle, new_angle, phase[N];
	
	for(int k = 0; k<N; ++k) {
		angle = tau*k;

		for(int n = 0; n<N; ++n) {
			new_angle = angle*n;

			real += data[n] * cos(new_angle);
			imag += data[n] * sin(new_angle);
		}

		sum_sqs = real*real + imag*imag;
		amp[k] = sqrt(sum_sqs);
		//phase[k] = atan(imag/real);

		real = 0; imag = 0;
	}

}

void fft(float* data, int N)
{
	int order[N];
	float sorted_data[N];
	float sorted_data_i[N];
	int rev = 0;
	int log_N = N;

	while (log_N > 0)
	{
		log_N >>= 1;
		if (log_N != 0)
		{
			NUMBER_OF_STAGES += 1;
		}
	}

	for (int n=0; n<N; ++n)
	{
		order[n] = 0;
		sorted_data[n] = 0;
		sorted_data_i[n] = 0;
	}

	for (int n = 0; n<N; ++n)
	{
		rev = reverse_bits(n, N);
		order[n] = rev;
	}

	for (int n = 0; n<N; ++n)
	{
		sorted_data[n] = data[order[n]];
	}

	// INTIALIZATION 
	int n = 0;
	int inc = 1;
	int k = 0;				// index for frequency data 
	int k_step = 0;
	int count = 0;

	float const_exp = 2.0*M_PI/N;

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
				n+=1;
			}
		}

		inc = inc * 2; 
		k_step = N/(2*inc);
		n = 0;
	}

	for (int n=0; n<N; ++n)
	{
		float temp = sorted_data[n]*sorted_data[n] + 
					  sorted_data_i[n]*sorted_data_i[n];
		temp = sqrt(temp);
		data[n] = temp;
		//printf("%f\n", temp);
	}

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

/*
void inverse_fft(struct Complex* input, struct Complex* output, int N)
{
	double inverse_N = 1 / (double)N;

	for (int i=0; i<N; ++i) 
	{
		input->i[i] *= -1;	// intended to be the complex conjugate of the data
	}

	double_complex_fft(input, output, N);

	for (int i=0; i<N; ++i)
	{
		output->r[i] *= inverse_N;
		output->i[i] *= inverse_N;

		//printf("%f\n", output->r[i]);
	}
}
*/

int reverse_bits(int num, int N)
{
	/* return bit reversed order */

	int rev = 0;
	int count = 0;
	int max_count = NUMBER_OF_STAGES;

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

void wave_gen(float *data, int N, float freq) { 
	//float freq = 1.0;						// cycles per second 
	int s_freq = N;
	float time = 1.0;		// seconds 
	float w = 2*M_PI*(freq/s_freq);		// angular frequency in sample space

	// "acquire" the data
	//printf("Acquired Data:\n");
	for (int n = 0; n<N; ++n)
	{
		data[n] = sin(w*time*n);// + 0.5*sin(w*time*n/2);	
		//printf("%f\n", data[n]);
	}
	//printf("\n");
}
