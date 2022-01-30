# ExploreDSP
Exploring Digital Signal Processing 

## Systems Requirements
GCC / Linux 

## File Structure
The modules folder contains:
1. fft.h/fft.c - Radix-2 FFT algorithm
2. filters.h/filters.c - Different comb filters and a couple reverberation algorithms
3. wav.h/wav.c - Reads and writes from/to WAV format
4. dataSim.h/dataSim.c - Simulates waveforms

The applications folder contains:
1. const-audio-waves - creating basics chord WAV files from simulated waveforms
2. fft-demo - demonstration of the FFT algorithm with intentions of being educational
3. reverberation - user friendly terminal app that can add a variety of effects to a user-inputed WAV file

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


