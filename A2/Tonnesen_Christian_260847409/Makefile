mysh: shell.c interpreter.c shellmemory.c
	gcc -g -c shell.c interpreter.c shellmemory.c header.h
	gcc -o mysh shell.o interpreter.o shellmemory.o header.h

clean: 
	rm mysh; rm *.o
