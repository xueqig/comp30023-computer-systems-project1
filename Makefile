allocate: allocate.c
	gcc -Wall -o allocate -g -fsanitize=address allocate.c
