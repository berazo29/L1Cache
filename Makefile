all : first

main : first.c first.h
	gcc -Wall -Werror -fsanitize=address -std=c11 first.c -o first
clean :
	rm first