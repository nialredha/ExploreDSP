# Understanding the Discrete and Fast Fourier Transform


## Foreword
Before reading this document, I highly encourage you watch the 3B1B video
on the fourier transform. I will be using a lot of the language used in that 
video, especially when attempting to explain the concepts behind the algorithm
(e.g. the idea of winding/walking a signal around a circle). 

Not to mention, Grant Sanderson (3B1B) is an incredible educator who builds 
elegant visuals and explanations to help understand notoriously challenging 
concepts. So you should be watching his videos regardless of if it helps you 
understand what I have to say here. Anyways, click on the image below to 
watch his video.


[![3B1B Fourier Transform](https://img.youtube.com/vi/spUNpyF58BY/0.jpg)](https://www.youtube.com/watch?v=spUNpyF58BY)


## Introduction
Assuming you have watched the video, here is what I am going to be talking
about. 

Overall, I am going to walk through what the fourier transform is and how it
can be descritized to run on a computer. However, I am not going to speak on
why it's useful or how it can be applied - mainly because I think the math is 
interesting in itself. 

First, I am going to start by briefly going over what the Fourier Transform is
trying to accomplish and how it goes about doing so in continuous space. 

Afterwards, I will move into discrete space and talk about how you can compute
the transform along with the pitfalls of doing so.

Finally, I will discuss how to overcome the major pitfall of the DFT - i.e.
computation time - using the FFT.

For the time being, I do not see this resource as educational for anyone but
me. Ultimately, however, my goal is to explain these topics in a way that is
helpful and interesting to other people.

If you somehow stumble upon this, please let me know what you think! 

## Fourier Transform


## Discrete Fourier Transform 
As you can see, the transformed data has two peaks: one at index 1 of the
array and one at index 7 of the array. Now, you might be asking... how does
this work? What do these peaks represent? Why are there two peaks? Let's talk
about it.  

The main idea used to transform time-domain data into the frequency-domain is
to walk the signal around a circle at different frequencies.
	
For example, when computing the transformed value for the first index, we walk
the signal around a circle at a frequency of 0Hz, meaning we don't walk the 
signal at all! Instead, we simply add up all of the wave amplitudes at each
point. So, why does that give us 0? This is because the input signal is a pure
sine wave, which oscillates at a frequency greater than 0Hz. Thus, the
amplitudes of the sine wave sum to 0 when walking the signal around a circle 
at a frequency of 0.
	
Alright, now let's look at the next index. Here we walk the signal around a 
signal at a frequency of 1Hz. Because we are walking at the same frequency of
the original sine wave, the points we sample and place on a circle all lie on
the positive x-axis. Meaning, if we were to sum the vectors (which 3B1B
relates to finding the location of the center of mass of the shape drawn by
walking the signal around a circle), we would get a very non-zero value.

Nevertheless, this is what is conceptually happening. We wind or walk a
complicated (or simple) signal at varying frequencies and determine
frequencies where the sum of the vectors created when walking the signal
around a circle is significantly non-zero. These frequencies are what were
present in the original signal. Thus, we are able to construct a lossless, 
tranformed signal that can tell you the frequencies present in the original
signal along with their phase and amplitude.

**DFT EXPLANATION NEEDS WORK. 


## Fast Fourier Transform 



  
