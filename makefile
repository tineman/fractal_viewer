fractals_mb : main.c helper.o
	gcc -O2 -Wall -Wextra -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 helper.o main.c -o fractals_mb

helper.o : helper.c
	gcc -c helper.c -Wall -Wextra

clean :
	rm -f *.o fractals_mb

.PHONY: clean
