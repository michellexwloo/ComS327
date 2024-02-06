poke327_102: Assignment1.02.o terrain.o
	gcc Assignment1.02.o terrain.o -o poke327_102

Assignment1.02.o: Assignment1.02.c terrain.h
	gcc Assignment1.02.c -o Assignment1.02.o

terrain.o: terrain.c terrain.h
	gcc terrain.c -o terrain.o

clean:
	rm -f poke327_102 Assignment1.02.o terrain.o