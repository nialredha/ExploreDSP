#include "filters.h"

#define INPUT_SIZE 10

int main() 
{

	delay_line* DL;
	size_t delay_length = 3;

	float input[INPUT_SIZE] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
	
	float output = 0.0;
	
	DL = init_delay_line(delay_length);

	for (int i=0; i<INPUT_SIZE; ++i)
	{
		step_delay_line(DL, input[i], &output);
		printf("%f\n", output);
	}

	return 0;
}
