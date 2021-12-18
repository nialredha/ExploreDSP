# Convolution Reverb 
Goal: take an audio file and convolve it with an impulse signal, using the
FFT, to create a reverb effect. 

Currently: built a delay line and comb filters to now make a Schroeder 
reverberator. The plan is to move to convolution reverb afterwards. I will 
likely want to change the name of the application to something more general,
like "Reverberation". 

## Acronyms 
Here are some recurrent acronyms I use for file naming and within the .c files
themselves:

DL ------- Delay Line

FFCF ----- Feed-Forward Comb Filter

FBCF ----- Feed-Back Comb Filter

APCF ----- All-Pass Comb Filter

SR ------- Shroeder Reverberation  

## TODO
1. Consolidate the different comb filter implementations... a lof of repeated
   code. Not too worried about right now, so maybe later. 
2. Write universal normalization funciton for all filter outputs to prevent
   distortion.


