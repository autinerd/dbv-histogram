CC ?= gcc

prog:
	$(CC) *.c -O3 -Wall -Wextra -lm -o dbv-histogram
