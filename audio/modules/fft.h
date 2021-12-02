#ifndef FFT2_H_	// include guard
#define FFT_H_

typedef struct Complex
{
	double* i;
	double* r;
}Complex;

// function declarations 
void	dft(double* data, double* amp, int N);		// discrete fourier transform

void	fft(float* data, int N);					// radix-2 algorithm 
int		reverse_bits(int num, int N);				// bit reversed order calc
void	wave_gen(float *data, int N, float freq);	// wave generation for testing

void	inverse_fft(struct Complex *input, struct Complex *output, int N);						
void	double_complex_fft(struct Complex *input, struct Complex *output, int N);

#endif // FFT_H_
