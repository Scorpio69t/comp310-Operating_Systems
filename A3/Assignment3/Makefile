mysh: shell.c interpreter.c shellmemory.c
	gcc -g -D FRAMESIZE=$(framesize) -D VARSIZE=$(varmemsize) -c shell.c interpreter.c shellmemory.c PCB.h frame.h
	gcc -D FRAMESIZE=$(framesize) -D VARSIZE=$(varmemsize) -o mysh shell.o interpreter.o shellmemory.o PCB.h frame.h

clean: 
	rm mysh; rm *.o
