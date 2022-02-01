# Usage:
# make			# compile final executable binary
# make run_fft	# run fft executable
# make run_ifft	# run ifft executable
# make clean	# remove ALL binaries

BUILD_DIR = build/

LINKER_FLAG = -lm

WAV_MODULE = ../../modules/wav.c
FILTERS_MODULE = ../../modules/filters.c
FFT_MODULE = ../../modules/fft.c
COMPLEX_MODULE = ../../modules/complex.c

reverb: ${WAV_MODULE} ${FILTERS_MODULE} ${FFT_MODULE} ${COMPLEX_MODULE} audio_processor.c
	[ -d $(BUILD_DIR) ] || mkdir -p $(BUILD_DIR)
	gcc ${LINKER_FLAG} -o build/reverb ${WAV_MODULE} ${FILTERS_MODULE} ${FFT_MODULE} ${COMPLEX_MODULE} audio_processor.c

run: 
	./build/reverb

clean:
	@echo "Cleaning up..."
	rm build/*
