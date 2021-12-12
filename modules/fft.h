#ifndef FFT_H_	// include guard
#define FFT_H_

typedef struct Complex
{
	float* i;
	float* r;
} Complex;

// function declarations 
int		reverse_bits(int num, int N);	// bit reversed order calc

void	dft(float* data, float* amp, int N);			// discrete fourier 
void	cdft(Complex *input,  Complex *output, int N);	

void	fft(float* data, int N);						// radix-2 algorithm 
void	cfft(Complex *input, Complex *output, int N);

void	ifft(Complex *input, Complex *output, int N);						

#endif // FFT_H_
