fractals_mb : main.c gifenc.o helper.o
	gcc -O2 helper.o gifenc.o main.c -o fractals_mb

helper.o : helper.c helper.h
	gcc -c helper.c -O2

gifenc.o : gifenc.c
	gcc -c gifenc.c -O2

clean :
	rm -f *.o fractals_mb *.gif

.PHONY: clean
