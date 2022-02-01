/*
	To Build: gcc audio_processor.c -o processAudio ../../modules/wav.c ../../modules/fft.c ../../modules/filters.c -lm
	To Run: ./a.out
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../../modules/wav.h"
#include "../../modules/fft.h"
#include "../../modules/filters.h"

int STATE = 0;

int main() {

	int state = STATE;
	
	printf("\n");
	printf("Choose Effect:\n");
	
	printf("\n");
	printf("(1) Delay Line\n"
		   "(2) Feed-Forward Comb Filter\n"
		   "(3) Feed-Back Comb Filter\n"
		   "(4) All-Pass Comb Filter\n"
		   "(5) Shroeder Reverberator\n"
		   "(6) Convolution Reverb\n");

	printf("\n");
	printf("Enter number corresponding to effect: ");
	scanf("%X", &state);

	STATE = state;

	printf("\n");
	switch(state) {
		case 1:
			printf("You Chose The Delay Line\n");
			delay_line_main();
			break;
		case 2:
			printf("You Chose The Feed-Forward Comb Filter\n");
			ff_comb_filter_main();
			break;
		case 3:
			printf("You Chose The Feed-Back Comb Filter\n");
			fb_comb_filter_main();
			break;
		case 4:
			printf("You Chose The All-Pass Comb Filter\n");
			ap_comb_filter_main();
			break;
		case 5:
			printf("You Chose The Shroeder Reverberator\n");
			shroeder_reverberator_main();
			break;
		case 6:
			printf("You Chose The Convolution Reverb\n");
			convolution_reverb_main();
			break;
	}

	printf("\n");

	return 0;
}

