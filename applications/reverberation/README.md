# Reverberation 
Overall Goal: take an audio file and convolve it with an impulse signal, using 
the FFT and IFFT, to create a reverb effect. I would also like it to be user-
friendly down-the-line.

Currently: built a delay line and comb filters to make a Schroeder 
reverberator. This was generally for fun, but also to figure out WAV file IO.
I also have built an overlap-add convolution to create a reverb effect - this
algorithm is fairly expensive but was meant as a stepping stone to the fast 
convolution reverb algorithm. 

## Acronyms 
Here are some recurrent acronyms I use when file-naming and in the .c files
themselves:

DL ------- Delay Line

FFCF ----- Feed-Forward Comb Filter

FBCF ----- Feed-Back Comb Filter

APCF ----- All-Pass Comb Filter

SR ------- Shroeder Reverberation  

OACR ----- Overlap-Add Convolution Reverb

## TODO
1. Consolidate the different comb filter implementations... there is a lot of 
   repeated code. I am not too worried about it right now, but will be later. 
2. Write universal normalization funciton for all filter outputs to prevent
   distortion.
3. Write 2-Channel open-add convolution reverb (might not do this)

## Notes
Currently, the fast convolution reverb takes roughly 21 seconds through use of
the FFT/IFFT, while the open-add convolution reverb takes 4090 seconds to 
compute. That is almost 200 times faster! And I haven't even begun optimizing 
what's written - there is a lot in the FFT algorithm that can be pre-computed
and a lot that can be re-writtent to improve efficiency.

Even though 21 seconds is a significant amount of time, it is crazy to think
transforming two different audio files to the frequency domain, convolving
them, and transforming back to the time domain can be that fast! 

If you would like to compare the output files each  algorithm created, they are
located in the data file and are named "OACR\_Full.wav" and 
"FFTCR\_1Ch\_IMPMasonicLodge.wav". 
