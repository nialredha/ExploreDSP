# Usage:
# make			# compile final executable binary
# make run_fft	# run fft executable
# make run_ifft	# run ifft executable
# make clean	# remove ALL binaries

BUILD_DIR = build/

LINKER_FLAG = -lm
FFT_MODULE = ../../modules/fft.c
DATASIM_MODULE = ../../modules/dataSim.c

all: fft ifft

fft: ${FFT_MODULE} ${DATASIM_MODULE} fft_demo.c 
	[ -d ${BUILD_DIR} ] || mkdir -p ${BUILD_DIR}
	gcc ${LINKER_FLAG} -o build/fft fft_demo.c ${FFT_MODULE} ${DATASIM_MODULE}

ifft: ${FFT_MODULE} ${DATASIM_MODULE} ifft_demo.c 
	[ -d ${BUILD_DIR} ] || mkdir -p ${BUILD_DIR}
	gcc ${LINKER_FLAG} -o build/ifft ifft_demo.c ${FFT_MODULE} ${DATASIM_MODULE}

run_fft: 
	./build/fft

run_ifft:
	./build/ifft

clean:
	@echo "Cleaning up..."
	rm build/*
