#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "../../modules/dataSim.h"
#include "../../modules/wav.h"

int main() {

	data_obj d;	
	d.frequency[0] = 293.665;	// D  - Octave 4
	d.frequency[1] = 369.994;	// F# - Octave 4
	d.frequency[2] = 440.000;	// A  - Octave 4
	d.frequency[3] = 523.251;	// C  - Octave 5

	//d.frequency[0] = 440.0;
	//d.frequency[1] = 294.0;
	//d.frequency[2] = 349.0;
	//d.frequency[3] = 523.0;
	//d.frequency[4] = 146.8;
	//d.frequency[5] = 74.0;
	//d.frequency[6] = 74.5;

	d.num_frequencies = 4;
	d.sample_rate = 44100;
	d.num_samples = d.sample_rate*10;	/* 3 second duration */

	struct wav_info w;
    w.num_channels = 1;
    w.bits_per_sample = 16;
    w.sample_rate = d.sample_rate;
    w.num_samples = d.num_samples; 

	/* initialize data to zero */
	int_fast32_t data[d.num_samples];	
	for(int i=0; i<d.num_samples; i++) {
		data[i] = 0;
	}

    char* fname = "d7_test.wav";     /* filename to write to */
	printf("Preparing to write wave w/ following parameters to %s:\n",fname);
    print_wav_info(&w);
	printf("\n");

    FILE* fp = fopen(fname,"wb");
    /* it is important to use "wb" here on Windows systems so Windows knows to 
	treat it as binary... otherwise
    carriage returns are inserted and the size of the file will be larger than 
	expected! */
    if(!fp) {
        fprintf(stderr,"Error opening %s for output!\n",fname);
        return 1;
    }

    write_wav_hdr(&w,fp);

	/* Determine maximum integer expressable in w.bits_per_sample bits, assuming
       twos complement encoding of signed integers */
    int_fast32_t M;
    switch(w.bits_per_sample) {
    case 8:
      M=0x7F;		/* 127 */
      break;
    case 16:
      M=0x7FFF;		/* 32767 */
      break;
    case 24:
      M=0x7FFFFF;	/* 8388607 */
      break;
    case 32:
      M=0x7FFFFFFF;	/* 2147483647 */
      break;
    default:
      printf("Error: bits_per_sample must be 8,16,24, or 32.\n");
      return 0;
    }

   	wave_gen_i(&d, data, M);

    for(int n=0; n < w.num_samples; n++) {
		int_fast32_t sample = data[n];
		write_sample(&w,fp, &sample);
	}
	fclose(fp);
	
	// Make sure everything is okay
	fp = fopen(fname,"rb");
    read_wav_info(&w,fp);
    printf("\nReading back info from %s header:\n",fname);
    print_wav_info(&w);
    fclose(fp);

	return 0;
}
