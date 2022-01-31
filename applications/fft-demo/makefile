# Usage:
# make			# compile final executable binary
# make install	# run executable
# make clean	# remove ALL binaries

LINKER_FLAG = -lm
FFT_MODULE = ../../modules/fft.c
DATASIM_MODULE = ../../modules/dataSim.c

all: fft ifft

fft: ${FFT_MODULE} ${DATASIM_MODULE} fft_demo.c 
	gcc ${LINKER_FLAG} -o build/fft fft_demo.c ${FFT_MODULE} ${DATASIM_MODULE}

ifft: ${FFT_MODULE} ${DATASIM_MODULE} ifft_demo.c 
	gcc ${LINKER_FLAG} -o build/ifft ifft_demo.c ${FFT_MODULE} ${DATASIM_MODULE}

run_fft: 
	./build/fft

run_ifft:
	./build/ifft

clean:
	@echo "Cleaning up..."
	rm build/*
