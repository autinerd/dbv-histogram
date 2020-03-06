prog:
	gcc-8 *.c -O3 -Wall -Wextra -lm -ftree-vectorize -march=native -g -o dbv-histogram

prog-avx512:
	gcc-8 *.c -O3 -Wall -Wextra -lm -ftree-vectorize -mprefer-vector-width=512 -march=skylake-avx512 -o dbv-histogram-avx512
