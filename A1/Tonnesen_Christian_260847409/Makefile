mysh: shell.c interpreter.c shellmemory.c
	gcc -g -c shell.c interpreter.c shellmemory.c
	gcc -g -o mysh shell.o interpreter.o shellmemory.o

clean: 
	rm mysh; rm *.o
