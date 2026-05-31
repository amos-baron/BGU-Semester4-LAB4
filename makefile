all: hexeditplus task4

hexeditplus: task1.c
	gcc -m32 -g -Wall -o hexeditplus task1.c

task4: task4.c
	gcc -m32 -fno-pie -fno-stack-protector -o task4 task4.c

clean:
	rm -f hexeditplus task4
