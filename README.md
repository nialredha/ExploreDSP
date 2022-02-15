# ExploreDSP
Exploring Digital Signal Processing 

## Systems Requirements
GCC / Linux 

## File Structure
The modules folder contains:
1. fft.h/fft.c - Radix-2 FFT algorithm
2. filters.h/filters.c - Different comb filters and a couple reverberation algorithms
3. dataSim.h/dataSim.c - Simulates waveforms
4. wav.h/wav.c - Reads and writes from/to WAV format

The applications folder contains:
1. const-audio-waves - creating basic WAV audio files from simulated waveforms
2. fft-demo - demonstration of the FFT algorithm with intentions of being educational
3. reverberation - user friendly terminal app that can add a variety of effects 
to a user-inputed WAV file and, in the case of adding reverb, an impulse WAV file

## Summary
The two main things I took a look at are: 

1. Fourier Transforms (DFT, FFT, IFT)
2. Audio Filters: mainly reverberation (Shroeder and Slow/Fast Convolutions)

## FFT
I placed a lot of time, after writing the FFT (Radix-2) algorithm, into
understanding the fourier transform, discrete fourier transform, fast fourier 
transform algorithm, and the inverse fourier transform, with intentions to
develop a demo script and README that could serve as a reference for anyone 
else trying to understand. This is currently on the back-burner, since there
are a couple key characteristics of the FFT/IFFT I haven't taken the time to
understand.

## Important Note 
This is in regard to both the modules/wav.h and modules/wav.c files.

I used a barebones wav module - written by someone else - for reading and writing
wav files and added more functionality on top of it to allow me to read and store
wav data and have more IO abstraction. While the website I downloaded the original
module from seems to no longer exist, modules/wav.h has more information on who
wrote it.

## Left To Do
1. Rename audio\_processor.c
2. Optimization for FFT multiple calls of the same data size
3. Benchmark FFT
