all: hexeditplus

hexeditplus: task1.c
	gcc -m32 -g -Wall -o hexeditplus task1.c

clean:
	rm -f hexeditplus
