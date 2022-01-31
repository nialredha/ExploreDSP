# Usage:
# make			# compile final executable binary
# make install	# run executable
# make clean	# remove ALL binaries

harmonic: ../../modules/wav.c ../../modules/dataSim.c sinewave_audio.c
	gcc -o build/sinewave_audio ../../modules/wav.c ../../modules/dataSim.c sinewave_audio.c -lm

install: 
	./build/sinewave_audio

clean:
	@echo "Cleaning up..."
	rm build/*
