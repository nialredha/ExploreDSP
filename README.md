# DSP
Exploring Digital Signal Processing 


## Summary
Most of my efforts in this repo went into writing the fft module located in
the module folder. That said, I wrote a couple "modules" to explore DSP for 
audio. The three main things I took a look at are: 

1. Fourier Transforms (DFT, FFT, IFT)
2. Reverb using Convolutions (want to eventually use the FFT for this)  
3. Reading, writing .wav files

## Important Note 
I did not write the wav module for reading and writing .wav files. I liked 
this one much better than what I had previously wrote, so I decided to use 
it instead of waste my time on polishing my own. Look in modules/wav.h to 
see where I downloaded it from. 

## FFT
I placed a lot of time after writing the Radix-2 algorithm into understanding 
the fourier transform, discrete fourier transform, fast fourier transform 
algorithm, and the inverse fourier transform (currently still working on its
implementation). I am currently developing a demo script and a README that can
serve as a reference for anyone else trying to understand.  
