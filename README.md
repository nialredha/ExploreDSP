# ExploreDSP
Exploring Digital Signal Processing 


## Summary
Most of my efforts in this repo went into writing the fft module located in
the module folder. That said, I wrote a couple "modules" to explore DSP for 
audio. The three main things I took a look at are: 

1. Fourier Transforms (DFT, FFT, IFT)
2. Reverb using Convolutions (want to eventually use the FFT for this)  
3. Reading/writing WAV files

## FFT
I placed a lot of time, after writing the FFT (Radix-2) algorithm, into
understanding the fourier transform, discrete fourier transform, fast fourier 
transform algorithm, and the inverse fourier transform (still working on its
implementation). 

I am currently developing a demo script and a README that can serve as a 
reference for anyone else trying to understand.  

## Important Note 
This is in regard to both the modules/wav.h and modules/wav.c files.

I did not write the wav module for reading and writing .wav files, but I 
tweaked it to allow me to read and store wav data. The reason I didn't write 
my own is simply because I liked this one much better than what I had written. 
Unfortunately, the website I downloaded it from on longer exists, but look in 
modules/wav.h to find more information on who I believe wrote it. 


