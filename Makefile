all : first

first : first.c first.h
	gcc -Wall -Werror -fsanitize=address -std=c11 first.c -o first -lm
clean :
	rm first