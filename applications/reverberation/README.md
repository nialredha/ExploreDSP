# Reverberation 
Overall Goal: take an audio file and convolve it with an impulse signal, using 
the FFT and IFFT, to create a reverb effect. I would also like it to be user-
friendly.

Currently: I met my goal :)
1. I implemented a delay line and comb filters to make a Schroeder reverberator. 
This was generally for fun, but also to figure out WAV file IO.
2. I implemented an overlap-add convolution to create a reverb effect - this
algorithm is fairly expensive but was meant as a stepping stone to the fast
convolution reverb. 
3. I implemented a fast convolution, using the FFT and IFFT, to create a
reverb effect.
4. I created a user-friendly program titled audio\_processor.c that 
consolidates everything I have done. 

## Acronyms 
Here are some recurrent acronyms I use when file-naming and in the .c files
themselves:

DL ------- Delay Line

FFCF ----- Feed-Forward Comb Filter

FBCF ----- Feed-Back Comb Filter

APCF ----- All-Pass Comb Filter

SR ------- Shroeder Reverberation  

OACR ----- Overlap-Add Convolution Reverb

FFTCR ---- Fast Fourier Transform Convolution Reverb

## TODO
1. Optimize fast convolution and the FFT (e.g precompile bit-reversed indexes 
   for multiple FFT calls)
2. Run benchmarks on FFT
3. Truly understand why multiplication in frequency domain is equivalent to a
   convolution in time domain.  

## Next Steps
Moving forward, I would like to learn about some more complicated/interesting
effects and possibly make an interactive editing environment.  
