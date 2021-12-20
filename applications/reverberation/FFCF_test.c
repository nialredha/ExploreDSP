#include <stdlib.h>
#include "../../modules/filters.h"
#include "../../modules/dataSim.h"

//gcc ff_comb_filter_test.c ../../modules/filters.c ../../modules/dataSim.c -lm

int main() 
{
	data_obj Data;

	Data.frequency[0] = 1.0;	// 1Hz 
	Data.num_frequencies = 1;

	Data.sample_rate = 8;	
	Data.num_samples = Data.sample_rate*1;	// 1 second duration

	Data.data = (float*)malloc(Data.num_samples * sizeof(Data.data)); 

	wave_gen_f(&Data, Data.data);

	ff_comb_filter* FFCF;
	size_t delay_length = 3;
	
	float b0 = 0.25;	
	float bm = 0.75;

	float output = 0.0;
	
	FFCF = init_ff_comb_filter(delay_length, b0, bm);

	for (int i=0; i<Data.num_samples+delay_length; ++i)
	{
		step_ff_comb_filter(FFCF, Data.data[i], &output);
		printf("%f\n", output);
	}

	free(Data.data);

	return 0;
}
