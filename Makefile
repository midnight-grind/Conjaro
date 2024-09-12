# Makefile for compiling and running ncurses_example

# default target: compile and run
all:
	gcc -o conjaro conjaro.c -lncurses
	sudo ./conjaro
