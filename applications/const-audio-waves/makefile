# Usage:
# make			# compile final executable binary
# make install	# run executable
# make clean	# remove ALL binaries

LINKER_FLAG = -lm
DATASIM_MODULE = ../../modules/dataSim.c
WAV_MODULE = ../../modules/wav.c

harmonic: ../../modules/wav.c ../../modules/dataSim.c sinewave_audio.c
	gcc ${LINKER_FLAG} -o build/sinewave_audio ${WAV_MODULE} ${DATASIM_MODULE} sinewave_audio.c

install: 
	./build/sinewave_audio

clean:
	@echo "Cleaning up..."
	rm build/*
