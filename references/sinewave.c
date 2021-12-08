/* Write a sine wave in WAVE format using wav.h
   Compile with, e.g.
   gcc wav.c sinewave.c -o sinewave -lm
   Or, if you don't want to keep recompiling wav.c, do
   gcc -c wav.c
   once and then the following when you modify sinewave.c:
   gcc -c sinewave.c
   gcc -o sinewave wav.o sinewave.o -lm
*/

#include <stdint.h>
#include <math.h>
#include "modules/wav.h"

int main()
{ 
    struct wav_info w;
    w.num_channels=2;
    w.bits_per_sample=16;
    w.sample_rate=44100;
    w.num_samples=w.sample_rate*3; /* 3 second duration */

    double freq[] = {580.0,280.0}; /* frequencies (in Hz) by channel */
    char* fname = "sound.wav";     /* filename to write to */

    printf("Preparing to write sine wave with the following parameters to %s:\n",fname);
    print_wav_info(&w);
    printf("Frequencies by channel:");
    for(int c=0; c < w.num_channels; c++) printf(" %.0f Hz",freq[c]);

    FILE* fp = fopen(fname,"wb");
    /* it is important to use "wb" here on Windows systems so Windows knows to treat it as binary... otherwise
    carriage returns are inserted and the size of the file will be larger than expected! */
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
      M=0x7F;
      break;
    case 16:
      M=0x7FFF;
      break;
    case 24:
      M=0x7FFFFF;
      break;
    case 32:
      M=0x7FFFFFFF;
      break;
    default:
      printf("Error: bits_per_sample must be 8,16,24, or 32.\n");
      return 0;
    }

    // Generate and write the actual sine wave
    double s,t;
    double dM = (double) M;
    int_fast32_t sample[w.num_channels];
    for(int n=0; n < w.num_samples; n++) {
        t = ((double) n) / ((double) w.sample_rate);
        for(int c=0; c < w.num_channels; c++) {
            s = sin(freq[c]*t*2*M_PI);
            sample[c] = (int_fast32_t)(dM * s);
        }
        write_sample(&w,fp,sample);
    }

    fclose(fp);

    // Make sure everything looks okay...
    fp = fopen(fname,"rb");
    read_wav_info(&w,fp);
    printf("\nReading back info from %s header:\n",fname);
    print_wav_info(&w);
    fclose(fp);
    
    return 0;
}


