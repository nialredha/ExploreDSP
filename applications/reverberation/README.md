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
3. Write 2-Channel open-add convolution reverb (right now it's just 1 channel) 
4. Write Fast convolution reverb


