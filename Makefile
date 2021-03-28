allocate: allocate.c
	gcc -o allocate allocate.c -lm

clean:
	rm *.o allocate
