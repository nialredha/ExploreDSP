#ifndef COMPLEX_H_	// include guard
#define COMPLEX_H_

typedef struct Complex
{
	float* r;	// real component
	float* i;	// imaginary component
} Complex;

void complex_multiply(Complex *a, Complex *b, Complex *o, int length);

#endif // COMPLEX_H_
