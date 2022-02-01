#include "complex.h"

void complex_multiply(Complex *a, Complex *b, Complex *o, int length)
{
	for (int i=0; i<length; i++)
	{
		o->r[i] = (a->r[i]*b->r[i]) - (a->i[i]*b->i[i]);
		o->i[i] = (a->r[i]*b->i[i]) + (a->i[i]*b->r[i]);
	}		
}
