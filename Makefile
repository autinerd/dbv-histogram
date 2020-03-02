CC ?= gcc

prog:
	$(CC) *.c -O3 -Wall -Wextra -lm -ftree-vectorize -march=native -o dbv-histogram

prog-avx512:
	$(CC) *.c -O3 -Wall -Wextra -lm -ftree-vectorize -mprefer-vector-width=512 -march=skylake-avx512 -o dbv-histogram-avx512
