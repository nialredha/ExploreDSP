#ifndef FFT2_H_	// include guard
#define FFT_H_

typedef struct Complex
{
	double* i;
	double* r;
} Complex;

// function declarations 
int		reverse_bits(int num, int N);				// bit reversed order calc

void	dft(float* data, float* amp, int N);		// discrete fourier transform
void	fft(float* data, int N);					// radix-2 algorithm 
void	inverse_fft(struct Complex *input, struct Complex *output, int N);						

// Soon to either be deleted or restored
void	wave_gen(float *data, int N, float freq);	// wave generation for testing
void	double_complex_fft(struct Complex *input, struct Complex *output, int N);

#endif // FFT_H_
