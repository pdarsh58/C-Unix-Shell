myshell: main.o prog_execution.o redirection.o bgProcess.o
	gcc -o myshell main.o prog_execution.o redirection.o bgProcess.o -Wall -Werror

main.o: main.c tools.h 
	gcc -c main.c -Wall -Werror

prog_execution.o: prog_execution.c
	gcc -c prog_execution.c -Wall -Werror


redirection.o: redirection.c
	gcc -c redirection.c -Wall -Werror

bgProcess.o: bgProcess.c
	gcc -c bgProcess.c -Wall -Werror

clean:
	rm -rf *.o myshell
	





