fractals_mb : main.c gifenc.o helper.o
	gcc -O2 -Wall -Wextra -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 helper.o gifenc.o main.c -o fractals_mb

helper.o : helper.c helper.h
	gcc -c helper.c -Wall -Wextra -O2

gifenc.o : gifenc.c
	gcc -c gifenc.c -Wall -Wextra -O2

clean :
	rm -f *.o fractals_mb *.gif

.PHONY: clean
