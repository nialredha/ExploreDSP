/* 
	The goal is to successfully take audio in a .wav file I made and apply a 
	reverberation effect using the FFT I built. This is all I want for 
	Christmas - let the 12 days commense. 

	Currently, I have made the audio and save it to a .wav file (see the 
	const-audio-waves folder if you are interested). I also have built the 
	FFT and the IFFT (see the fft-demo folder if you are interested). 

	All that is left for me to do is figure out how to use the FFT for 
	convolution reverbs...
		
		Realistically, I am going to need to add a bunch of functionality to
		the filter module. Right now it looks like a delay line and comb 
		is what's in store for me right now. Shouldn't take too long, as long
		as I don't waste my time writing things that are ultimately going to 
		be delted :)
*/

#include <stdlib.h>

#include "../../modules/filters.h"
#include "../../modules/wav.h"
#include "../../modules/fft.h"

void find_max_int(struct wav_info* w, int* max_int) {
/* determine maximum integer expressable in w.bits_per_sample bits, assuming
twos complement encoding of signed integers */

    switch(w->bits_per_sample) {
    case 8:
      *max_int=0x7f;		/* 127 */
      break;
    case 16:
      *max_int=0x7fff;		/* 32767 */
      break;
    case 24:
      *max_int=0x7fffff;	/* 8388607 */
      break;
    case 32:
      *max_int=0x7fffffff;	/* 2147483647 */
      break;
    default:
      printf("error: bits_per_sample must be 8,16,24, or 32.\n");
    }
}

void main() {

	struct wav_info input_info;
	struct wav_info impulse_info;

    char* input_file = "data/input_signals/harmonic.wav";	
	char* impulse_file = "data/impulse_responses/Masonic Lodge.wav";

    FILE* input = fopen(input_file,"rb");
	FILE* impulse = fopen(impulse_file, "rb");
	
	read_wav_info(&input_info, input);
	read_wav_info(&impulse_info, impulse);

	printf("Input:\n");
    print_wav_info(&input_info);
	printf("\nImpulse:\n");
    print_wav_info(&impulse_info);
	printf("\n");

	uint32_t max_input_int;				/* used to convert wav data to float */
	uint32_t max_impulse_int;			/* used to convert wav data to float */

	printf("Sample Number: %d\n", input_info.num_samples);

	int16_t input_idata[input_info.num_samples];
	int16_t impulse_idata[impulse_info.num_samples];

	float input_fdata[input_info.num_samples];
	float impulse_fdata[impulse_info.num_samples];

	find_max_int(&input_info, &max_input_int);
	find_max_int(&impulse_info, &max_impulse_int);

	read_wav_data(&input_info, input_idata, input); 
	read_wav_data(&impulse_info, impulse_idata, impulse); 

    fclose(input);
    fclose(impulse);

	for(int i=0; i<input_info.num_samples; i++) {
		input_fdata[i] = ((float)input_idata[i]/(float)max_input_int);
		//printf("%f\n", input_fdata[i]);
	}
	for(int i=0; i<impulse_info.num_samples; i++) {
		impulse_fdata[i] = ((float)impulse_idata[i]/(float)max_impulse_int);
		//printf("%f\n", impulse_fdata[i]);
	}

	//printf("max int = %d \n", max_int);

	/*
	float input[] = {2.0, 4.0, 3.0, 6.0};
	float impulse[] = {1.0, 5.0, 2.0, 3.0, 4.0};
	float output[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	*/

	int input_length = input_info.num_samples;
	int impulse_length = impulse_info.num_samples;
	//printf("%d\n", input_length);
	//printf("%d\n", impulse_length);

	int output_length = input_length + impulse_length - 1;	

	struct wav_info output_info;

    output_info.num_channels = 1;
    output_info.bits_per_sample = 16;
    output_info.sample_rate = 44100;
    output_info.num_samples = output_length; 

	float output_fdata[output_length];
	for(int i=0; i<output_info.num_samples; i++) {
		output_fdata[i] = 0.0;
	}

	convolution_reverb(input_fdata, impulse_fdata, output_fdata, 
					   input_length, impulse_length);

	
	char* output_file = "data/reverb.wav";	// filename to write to
	printf("Preparing to write to %s:\n",output_file);
    print_wav_info(&output_info);
	printf("\n");

    FILE* output = fopen(output_file,"wb");
	if(!output) {
        fprintf(stderr,"Error opening %s for output!\n",output_file);
    }

	int max_output_int;
	find_max_int(&output_info, &max_output_int);

	float max_output_float = (float)max_output_int;
	//printf("%f\n", max_output_float);

    write_wav_hdr(&output_info,output);

	for(int n=0; n < output_info.num_samples; n++) {
		uint_fast16_t sample = (uint_fast16_t)(output_fdata[n]*max_output_float);
		//printf("%f\n", output_fdata[n]);
		write_sample(&output_info, output, &sample);
	}

	fclose(output);
	
	// Make sure everything is okay
	FILE* fp = fopen(output_file,"rb");
    read_wav_info(&output_info,fp);
    printf("\nReading back info from %s header:\n",output_file);
    print_wav_info(&output_info);
    fclose(fp);

	/*
	for (int k=0; k<output_length; k++) {
		printf("%f \n", output[k]);
	}
	*/
}

 
