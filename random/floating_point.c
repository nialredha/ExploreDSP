// Floating Point Addition and Multiplication

// References: 
// https://www.doc.ic.ac.uk/~eedwards/compsys/float/
// https://github.com/geohot/twitchcore/blob/master/tf32.c

#include <stdio.h>
#include <stdint.h>

#define S(x) (x>>31) // MSB is the sign bit
#define E(x) ((x>>23) & ((1<<8) - 1)) // Next 8 bits is ecponent 
#define M(x) (x & ((1<<23) - 1)) // Next 23 bits is mantissa 

int add(int fa, int fb)
{
	int greater_exp = (E(fa) > E(fb)) ? E(fa) : E(fb);

	int new_fa_mantissa = ((1<<23)|M(fa))>>(greater_exp - E(fa));
	int new_fb_mantissa = ((1<<23)|M(fb))>>(greater_exp - E(fb));

	//printf("S(fa) = %d\n", S(fa) ? -1 : 1);

	// account for sign before adding the mantissa's 
	new_fa_mantissa *= S(fa) ? -1 : 1;
	new_fb_mantissa *= S(fb) ? -1 : 1;
	
	int result_mantissa = new_fa_mantissa + new_fb_mantissa;

	// record the sign and remove it from resultant mantissa 
	int new_sign = result_mantissa<0;
	result_mantissa *= new_sign ? -1 : 1;

	int shift = result_mantissa>>23;
	printf("shift = %d\n", shift);

	if (shift == 0) { result_mantissa<<=1; greater_exp -= 1; }
	else if (shift == 3) { result_mantissa>>=1; greater_exp +=1; }
	
	return (new_sign<<31) | ((greater_exp&((1<<8) - 1))<<23) | (result_mantissa&((1<<23) - 1));
}

int multiply(int fa, int fb) 
{
	int new_sign = S(fa) ^ S(fb); // XOR: if one is negative the product is
	int new_exponent = E(fa) + E(fb) - 127; // add bias twice subtract once
	int new_mantissa = ((uint64_t)((1<<23)|M(fa))) * 
					   ((uint64_t)((1<<23)|M(fb))) >> 23;

	return (new_sign<<31) | 
		   ((new_exponent&((1<<8) - 1)) << 23) |
		   (new_mantissa&((1<<23) - 1));
}

void float_point_rep(int x) 
{
	printf("    Sign: %d\n", S(x));
	printf("Exponent: %d\n", E(x));
	printf("Mantissa: %d\n", M(x));
	printf("\n");
}

void main() 
{
	float a = 1.3;
	float b = 4.5;
	float c = a*b;
	float d = a+b;

	int fa = *(uint32_t*)(&a);
	int fb = *(uint32_t*)(&b); 
	int fc = *(uint32_t*)(&c);
	int fd = *(uint32_t*)(&d); 

	printf("\n");
	printf("%f * %f = %f\n", a, b, c);
	printf("%x * %x = %x\n", fa, fb, fc);
	printf("\n");
	printf("%f + %f = %f\n", a, b, d);
	printf("%x + %x = %x\n", fa, fb, fd);
	printf("\n");

	float_point_rep(fa);
	float_point_rep(fb);
	float_point_rep(fc);
	float_point_rep(fd);

	//int result = multiply(fa, fb);
	int result = add(fa, fb);

	float_point_rep(result);

	printf("%f + %f = %f\n", a, b, *((float*)&result));
	printf("\n");
}
