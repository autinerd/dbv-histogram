CC ?= gcc

prog:
	$(CC) *.c -O3 -Wall -Wextra -o dbv-histogram
