#include <stdlib.h>
#include "wav.h"

int POINTING_AT_DATA = 0;		// used to determine if read_wav_info was called

void read_wav_info(wav_info *w, FILE *fp) {

	// To be read from *fp
	uint32_t data_size;
	uint32_t byte_rate;
	uint16_t block_align;

	uint8_t x[4];	// buffer for reading from *fp

	// Start reading from beginning of *fp
	if(fseek(fp,0,SEEK_SET)) // returns 1 if unsuccessful 
	{
		fprintf(stderr,"Error with fseek in read_wav_info in wav.c\n");
		exit(EXIT_FAILURE);
	}

	// First four bytes of any RIFF file should be the ASCII codes for "RIFF"
	fread(x,1,4,fp);
	if((x[0] != 0x52) || (x[1] != 0x49) || (x[2] != 0x46) || (x[3] != 0x46)) 
	{
		fprintf(stderr,"Error: First 4 bytes indicate file is not a RIFF/WAVE file!\n");
		exit(EXIT_FAILURE);
	}

	// Next four bytes give the RIFF chunk size RCS in Little Endian format
	fread(x,1,4,fp);
	// We're not going to do anything with it, but you could do
	// uint32_t RCS = x[0] | (x[1] << 8) | (x[2] << 16) | (x[3] << 24);
	// here if you wanted to...

	// Next four bytes should be the ASCII codes for "WAVE"
	fread(x,1,4,fp);
	if((x[0] != 0x57) || (x[1] != 0x41) || (x[2] != 0x56) || (x[3] != 0x45)) 
	{
		fprintf(stderr,"Error: Bytes 9-12 indicate file is not a RIFF/WAVE file!\n");
		exit(EXIT_FAILURE);
	}

	// Look for the "fmt " subchunk of this RIFF file...
	while(1) 
	{
		fread(x,1,4,fp);

		// See if the four bytes we just read are the ASCII codes for "fmt "
		if((x[0] == 0x66) && (x[1] == 0x6D) && (x[2] == 0x74) && (x[3] == 0x20)) 
		{
			// Found the "fmt " subchunk

			// The next four bytes should give the size of the fmt  subchunk
			// in Little Endian.  This should be 16 if this is a PCM WAVE file.
			fread(x,1,4,fp);
			uint32_t y = x[0] | (x[1] << 8) | (x[2] << 16) | (x[3] << 24);
			if(y != 16) 
			{
				fprintf(stderr,"Error: File does not appear to be a PCM RIFF/WAVE file.\n");
				fprintf(stderr,"fmt  subchunk doesn't have size 16.\n");
				exit(EXIT_FAILURE);
			}

			// Next two bytes should give the integer 1 (for PCM format) in Little Endian
			fread(x,1,4,fp);
			uint16_t wFormatTag = x[0] | (x[1] << 8);
			if(wFormatTag != 1) 
			{
				fprintf(stderr,"Error: File does not appear to be a PCM RIFF/WAVE file.\n");
				fprintf(stderr,"wFormatTag is not equal to 1.\n");
				exit(EXIT_FAILURE);
			}

			// The rest of the fmt  subchunk should give num_channels (as a two-byte
			// integer in L.E.), sample_rate (four-byte L.E.), byte_rate (four-byte L.E.),
			// block_align (two-byte L.E.), and bits_per_sample (two-byte L.E.)
			w->num_channels = x[2] | (x[3] << 8);

			fread(x,1,4,fp);
			w->sample_rate = x[0] | (x[1] << 8) | (x[2] << 16) | (x[3] << 24);

			fread(x,1,4,fp);
			byte_rate = x[0] | (x[1] << 8) | (x[2] << 16) | (x[3] << 24);

			fread(x,1,4,fp);
			block_align = x[0] | (x[1] << 8);
			w->bits_per_sample = x[2] | (x[3] << 8);

			// Now we're done with the fmt  subchunk
			break;
		}

		// The four bytes after the four-byte "Chunk ID" in any RIFF file give
		// the size of the chunk as a four-byte integer in Little Endian
		uint32_t chunk_size;
		fread(x,1,4,fp);
		chunk_size = x[0] | (x[1] << 8) | (x[2] << 16) | (x[3] << 24);

		// Skip over this subchunk and keep looking for "fmt" subchunk
		if(fseek(fp,chunk_size,SEEK_CUR)) 
		{
			fprintf(stderr,"Error: Couldn't find fmt subchunk in file.\n");
			exit(EXIT_FAILURE);
		}
	}

	// Now look for the "data" subchunk of this RIFF file...
	while(1) 
	{
		fread(x,1,4,fp);

		// See if these four bytes are the ASCII codes for "data"
		if((x[0] == 0x64) && (x[1] == 0x61) && (x[2] == 0x74) && (x[3] == 0x61)) 
		{
			// Found the "data" subchunk
			fread(x,1,4,fp);
			data_size = x[0] | (x[1] << 8) | (x[2] << 16) | (x[3] << 24);

			POINTING_AT_DATA = 1;	// seek position is at beginning of data 

			// Now we're done reading from *fp...
			break;
		}

		// The four bytes after the four-byte "Chunk ID" in any RIFF file give
		// the size of the chunk as a four-byte integer in Little Endian
		uint32_t chunk_size;
		fread(x,1,4,fp);
		chunk_size = x[0] | (x[1] << 8) | (x[2] << 16) | (x[3] << 24);

		// Skip over this subchunk and keep looking for "data" subchunk
		if(fseek(fp,chunk_size,SEEK_CUR)) 
		{
			fprintf(stderr,"Error: Couldn't find data subchunk in file.\n");
			exit(EXIT_FAILURE);
		}
	}

	// Determine num_samples
	w->num_samples = data_size/((w->num_channels)*(w->bits_per_sample)/8);

	// Do some error checking:
	if(block_align != (w->num_channels)*(w->bits_per_sample)/8) 
	{
		fprintf(stderr,"Error: block_align, num_channels, bits_per_sample mismatch in WAVE header.\n");
		fprintf(stderr,"block_align=%i\n",block_align);
		fprintf(stderr,"num_channels=%i\n",(int) w->num_channels);
		fprintf(stderr,"bits_per_sample=%i\n",(int) w->bits_per_sample);
		exit(EXIT_FAILURE);
	}
	if(byte_rate != (w->sample_rate)*(w->num_channels)*(w->bits_per_sample)/8) 
	{
		fprintf(stderr,"Error: byte_rate, sample_rate, num_channels mismatch in WAVE header.\n");
		fprintf(stderr,"byte_rate=%i\n",byte_rate);
		fprintf(stderr,"sample_rate=%i\n",(int) w->sample_rate);
		fprintf(stderr,"num_channels=%i\n",(int) w->num_channels);
		exit(EXIT_FAILURE);
	}
   
}

void read_wav_data(struct wav_info *w, uint16_t *wav_data, FILE *fp) {

	if(POINTING_AT_DATA != 1)
	{
		return;
	}

	POINTING_AT_DATA = 0;	// reset

	size_t num_of_samples = w->num_samples * w->num_channels;
	uint32_t bytes_per_sample = w->bits_per_sample / 8;
	uint16_t y[w->num_channels];	// buffer for reading from fread

	for (int n=0; n<w->num_samples; n++) 
	{
		fread(y, bytes_per_sample, w->num_channels, fp);
		wav_data[n] = y[0];
		if (w->num_channels > 1)
		{
			for (int i=1; i<w->num_channels; i++)
			{
				// channels are separated into their respective chuncks
				wav_data[n+(w->num_samples*i)] = y[i];	
			}
		}
	}
}

void write_wav_hdr(const struct wav_info *w, FILE *fp) {

	// We'll need the following:
	uint32_t data_size = (w->num_samples)*(w->num_channels)*(w->bits_per_sample)/8;
	uint32_t RCS = data_size+36;
	uint32_t byte_rate = (w->sample_rate)*(w->num_channels)*(w->bits_per_sample)/8;
	uint16_t block_align = (w->num_channels)*(w->bits_per_sample)/8;

	// Prepare a standard 44 byte WAVE header from the info in w
	uint8_t h[44];
	// Bytes 1-4 are the ASCII codes for the four characters "RIFF"
	h[0]=0x52;
	h[1]=0x49;
	h[2]=0x46;
	h[3]=0x46;
	// Bytes 5-8 are RCS (i.e. data_size plus the remaining 36 bytes in the header)
	// in Little Endian format
	for(int i=0; i<4; i++) h[4+i] = (RCS >> (8*i)) & 0xFF;
	// Bytes 9-12 are the ASCII codes for the four characters "WAVE"
	h[8]=0x57;
	h[9]=0x41;
	h[10]=0x56;
	h[11]=0x45;
	// Bytes 13-16 are the ASCII codes for the four characters "fmt "
	h[12]=0x66;
	h[13]=0x6D;
	h[14]=0x74;
	h[15]=0x20;
	// Bytes 17-20 are the integer 16 (the size of the "fmt " subchunk
	// in the RIFF header we are writing) as a four-byte integer in 
	// Little Endian format
	h[16]=0x10;
	h[17]=0x00;
	h[18]=0x00;
	h[19]=0x00;
	// Bytes 21-22 are the integer 1 (to indicate PCM format),
	// written as a two-byte Little Endian
	h[20]=0x01;
	h[21]=0x00;
	// Bytes 23-24 are num_channels as a two-byte Little Endian
	for(int j=0; j<2; j++) h[22+j] = (w->num_channels >> (8*j)) & 0xFF;
	// Bytes 25-26 are sample_rate as a four-byte L.E.
	for(int i=0; i<4; i++) h[24+i] = (w->sample_rate >> (8*i)) & 0xFF;
	// Bytes 27-30 are byte_rate as a four-byte L.E.
	for(int i=0; i<4; i++) h[28+i] = (byte_rate >> (8*i)) & 0xFF;
	// Bytes 31-34 are block_align as a two-byte L.E.
	for(int j=0; j<2; j++) h[32+j] = (block_align >> (8*j)) & 0xFF;
	// Bytes 35-36 are bits_per_sample as a two-byte L.E.
	for(int j=0; j<2; j++) h[34+j] = (w->bits_per_sample >> (8*j)) & 0xFF;
	// Bytes 37-40 are the ASCII codes for the four characters "data"
	h[36]=0x64;
	h[37]=0x61;
	h[38]=0x74;
	h[39]=0x61;
	// Bytes 41-44 are data_size as a four-byte L.E.
	for(int i=0; i<4; i++) h[40+i] = (data_size >> (8*i)) & 0xFF;

	// Write the header to the beginning of *fp
	if(fseek(fp,0,SEEK_SET)) {
		fprintf(stderr,"Error with fseek in write_wav_header in wav.c\n");
		exit(EXIT_FAILURE);
	}
	fwrite(h,1,44,fp);
}

void print_wav_info(const wav_info *w) {

	printf("Number of channels: %i\n",(int) w->num_channels);
	printf("Bits per sample: %i\n",(int) w->bits_per_sample);
	printf("Sample rate: %i Hz\n",(int) w->sample_rate);
	printf("Total samples per channel: %i\n", (int) w->num_samples);

	int duration = w->num_samples / w->sample_rate;
	int r = w->num_samples % w->sample_rate;	// remainder
	if(r==0) 
	{ 
		printf("Duration: %i s\n", duration); 
	}
	else 
	{
		printf("Duration: %i + %i/%i s\n", duration, r, (int) w->sample_rate);
	}
}

void write_sample(const wav_info* w, FILE* fp, const int16_t* sample) {

	// We'll assume w->bits_per_sample is divisible by 8, otherwise
	// one should do bytes_per_sample++ and make sure
	// the last (w->bits_per_sample % 8) bits of each sample[i] are zero
	int bytes_per_sample = w->bits_per_sample / 8;	
	uint8_t x[bytes_per_sample];	// each byte is 8 bits thus uint8

	for(int i=0; i<w->num_channels; i++) 
	{
		// populate x with sample[i] in Little Endian format, then write it
		for(int j=0; j<bytes_per_sample; j++) 
		{
			// Bit shift right 8 times then & w/ 0xFF to drop remaining MSB's
			x[j] = (sample[i] >> (8*j)) & 0xFF;	
		}

		fwrite(x,1,bytes_per_sample,fp);
	}
}

float* retrieve_data(wav_info *w) {

	int16_t *idata;

	// prompt user to enter input file path. If the file is valid, open it and
	// read the data into the idata buffer.
	printf("\n");
	printf("Enter relative file path to .wav file: ");
	idata = open_file_read_data(w);

	if(idata == NULL) { return NULL; }

	// print input file wav header info
	printf("\n");
	printf("WAV File Header Info:\n");
	printf("-----------------------------------\n");
    print_wav_info(w);
	printf("-----------------------------------\n");
	printf("\n");

	// initialize pointer to buffer containing input data of type float,
	// convert the integer data to float, and store it in the fdata buffer.
	// free idata's memory since we will only be using fdata from now on. 
	float *fdata;
	fdata = convert_data_to_float(w, idata);
	free(idata);

	if(fdata == NULL) { return NULL; }

	return fdata;
}

int16_t* open_file_read_data(wav_info *w) {

	// user input relative file path to .wav file
    char file_path[1000];
	scanf("%s", file_path); 

	// char* file_path = "data/input_signals/DeChaka_Instrumental.wav";

	// loop to check file exists. The loop will not break until a correct 
	// file path has been inputed.
	while(1) 
	{
		FILE* file;

		// if it exists, close the file and break out of the loop
		if(file = fopen(file_path, "r"))
		{
			fclose(file);
			printf("\n");
			printf("File exists\n");

			break;
		}
		// else, print an error and prompt user to try again
		else
		{
			printf("\n");
			printf("Error: File does not exist\n");

			printf("\n");
			printf("Enter relative file path again: ");
			scanf("%s", file_path); 
		}
	}
	
	// open and read .wav file. "rb" means read binary 
    FILE* fp = fopen(file_path,"rb");

	// reads the header and inputs info into the wav_info struct
	read_wav_info(w, fp);

	// allocate data based on the number of samples and number of channels and
	// store the int data in the idata buffer. 
	int16_t *idata = (int16_t *)malloc(sizeof(int16_t)*(w->num_samples*w->num_channels));
	if(idata == NULL) { return NULL; }

	read_wav_data(w, idata, fp); 

	// now that the header and the data has been read and stored locally there
	// is no more need for the file. 
	fclose(fp);

	return idata;
}

float* convert_data_to_float(wav_info *w, int16_t* idata) {
/* find the largest expressable integer based on the .wav formatting, 
   specifically the amount of bits per sample */

	uint32_t max_int;
	find_max_int(w, &max_int);
	
	// allocate memory for float data buffer
	float *fdata = (float *)malloc(sizeof(float)*(w->num_samples*w->num_channels));

	if(fdata == NULL) { return NULL; }
	
	// divide each data point by max_int to convert the data into floats
	// ranging from -1 to 1.
	for(int i=0; i<w->num_samples*w->num_channels; i++) {

		fdata[i] = ((float)idata[i]/(float)max_int);
	}

	return fdata;
}

void create_file_write_data(wav_info *w, float *fdata) {

	// user input relative file path to .wav file
    char file_path[1000];
	scanf("%s", file_path); 

	// set output file path and print the .wav file header info
	printf("\n");
	printf("---------------------------------------------\n");
	printf("Preparing to write to %s:\n", file_path);
    print_wav_info(w);
	printf("---------------------------------------------\n");
	printf("\n");

	// open the output file and write the header info to the .wav file
    FILE* fp = fopen(file_path,"wb");
	if(!fp) {
        fprintf(stderr,"Error opening %s for output!\n",fp);
    }

    write_wav_hdr(w,fp);

	// compute the largest expressable integer based on the amount of bits
	// per sample.
	int max_int;
	find_max_int(w, &max_int);

	float max_float = (float)max_int;
	//printf("%f\n", max_output_float);

	// write the 2-channel data to a buffer of length 2, converting the float
	// data to int before storing it in the buffer. Afterwards, write the 2-
	// channel sampel using the write_sample() function.
	int16_t sample[2];
	for(int i=0; i < w->num_samples; i++) 
	{
		for(int j=0; j < w->num_channels; j++)
		{
			sample[j] = (int16_t)(fdata[i+(w->num_samples*j)]*max_float);
		}
		write_sample(w, fp, sample);
	}

	fclose(fp);
}

void find_max_int(wav_info* w, int* max_int) {
/* determine maximum integer expressable in w.bits_per_sample bits, assuming
twos complement encoding of signed integers */

    switch(w->bits_per_sample) {
    case 8:
      *max_int=0x7f;		/* 127 */
      break;
    case 16:
      *max_int=0x7fff;		/* 32767 */
      break;
    case 24:
      *max_int=0x7fffff;	/* 8388607 */
      break;
    case 32:
      *max_int=0x7fffffff;	/* 2147483647 */
      break;
    default:
      printf("error: bits_per_sample must be 8,16,24, or 32.\n");
    }
}

void normalize_data(float *fdata, int N) {
/* Finding maximum output value to "normalize" */

	float record = -1000.0;
	for (int i=0; i<N; i++)
	{
		// printf("%f\n", output_fdata[i]);

		if (abs(fdata[i]) > record)
		{
			record = abs(fdata[i]);
		}
	}
	// printf("%f\n", record); 

	for (int i=0; i<N; i++)
	{
		// printf("%f\n", output_fdata[i]);
		fdata[i] /= record + 1.0;	// "normalizing" the data
	}
}
