#ifndef WAV_H_	// include guard
#define WAV_H_

/* Provides basic handling of PCM format RIFF/WAVE audio files see:
   http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/Docs/riffmci.pdf
   for some information on the relevant specification.
   This is not as elaborate as the WAVE file support in the "audiofile" package
   but is very simple, self-contained, and probably adequate for most purposes.
*/


/* Note from me:**************************************************************

   This was written by http://oztekin.net/ !!
   I only slightly modified it by adding read_wav_data()

*****************************************************************************/

#include <stdint.h>
#include <stdio.h>
  
struct wav_info {
   uint_fast16_t num_channels;      /* 1 for mono, 2 for stereo, etc. */
   uint_fast16_t bits_per_sample;   /* 16 for CD, 24 for high-res, etc. */
   uint_fast32_t sample_rate;       /* 44100 for CD, 88200, 96000, 192000, etc. */
   uint_fast32_t num_samples;       /* total number of samples per channel */
};

void read_wav_info(struct wav_info* w, FILE* fp);
/* Read wav_info from *fp, assuming *fp is a PCM format RIFF/WAVE file.
   Leaves the seek position of *fp at the beginning of the data section
   of *fp, so one could immediately begin reading/writing samples */

void read_wav_data(struct wav_info* w, uint16_t *data, FILE* fp);
/* Read wav_info from *fp, assuming *fp is a PCM format RIFF/WAVE file.
   Leaves the seek position of *fp at the beginning of the data section
   of *fp, so one could immediately begin reading/writing samples */


void write_wav_hdr(const struct wav_info* w, FILE* fp);
/* Write a standard 44-byte PCM format RIFF/WAVE header to the beginning of *fp.
   Again, the seek position of *fp will be left at the beginning of
   the data section, so one can immediately begin writing samples */

void print_wav_info(const struct wav_info* w);
/* Prints information from *w to stdout */

void write_sample(const struct wav_info* w, FILE* fp, const int_fast16_t* sample);
/* Write a sample to *fp in the correct Little Endian format.
   sample should be an array with w->num_channels elements.
   Note that we use the int_fast32_t datatype to hold samples, which should be
   changed if you want to use bits_per_sample > 32.  Also, if you use, say,
   bits_per_sample=24, then you want to make sure that your actual samples
   are going to fit into a 3-byte Little Endian integer in twos-complement
   encoding.  If you're only going to use bits_per_sample=16, you could
   use int_fast16_t instead of int_fast32_t.  Note also that the WAVE
   file format expects 8-bit samples to be UNsigned, so if you're going
   to use bits_per_sample=8, then you should use uint_fast8_t to hold
   your samples. */

float* retrieve_data(struct wav_info *w);
/* prompts user to input file path, reads input data using 
   "open_file_read_data", updates and prints wav_info, converts data to
   floating point using "convert_data_to_float", and returns float data. */

int16_t* open_file_read_data(struct wav_info *w);
/* open .wav file, read int data into buffer and return stored data. */

float* convert_data_to_float(struct wav_info *w, int16_t* idata);
/* convert the int data into float using the max bits per sample stored in 
   .wav file header. */

void normalize_data(float *fdata, int N);
/* finding maximum output value to "normalize" */

void create_file_write_data(struct wav_info *w, float* fdata);
/* reads output file path, prints wav_info, writes header, converts floating
   point data back to int, and writes data to .wav. */

void find_max_int(struct wav_info* w, int* max_int);
/* Find the max integer a wav file can express - essentially figure out a wav
   file's bit-depth to determine the largest integer expressable. 
   
   For example, if the bit depth was 16 bits, the maximum integer expressable
   would be 32767 */

#endif // WAV_H_
