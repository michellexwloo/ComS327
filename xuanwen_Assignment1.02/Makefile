all: Assignment1.02.o terrain.o
	gcc Assignment1.02.o terrain.o -o poke327_102

Assignment1.02.o: Assignment1.02.c terrain.h
	gcc -Wall -Werror -ggdb Assignment1.02.c -c

terrain.o: terrain.c terrain.h
	gcc -Wall -Werror -ggdb terrain.c -c

clean:
	rm -f Assignment1.02.o terrain.o poke327_102 core *~ *.o