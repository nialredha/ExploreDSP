# ExploreDSP
Exploring Digital Signal Processing 


## Summary
Most of my efforts in this repo went into writing the fft/ifft located in
the module folder. That said, I wrote a couple other "modules" to explore DSP
for audio. The three main things I took a look at are: 

1. Fourier Transforms (DFT, FFT, IFT)
2. Reverberation (Shroeder and Slow/Fast FFT Convolutions)
3. Reading/writing WAV files

## FFT
I placed a lot of time, after writing the FFT (Radix-2) algorithm, into
understanding the fourier transform, discrete fourier transform, fast fourier 
transform algorithm, and the inverse fourier transform, with intentions to
develop a demo script and README that could serve as a reference for anyone 
else trying to understand. This is currently on the back-burner, since there
are a couple key characteristics of the FFT/IFFT I do not fully understand.

## Important Note 
This is in regard to both the modules/wav.h and modules/wav.c files.

I did not write the wav module for reading and writing .wav files, but
tweaked it to allow me to read and store wav data. The reason I didn't write 
my own is simply because I liked this one much better than what I had written. 
While the website I downloaded it from no longer exists, modules/wav.h has 
more information on who wrote it. 


