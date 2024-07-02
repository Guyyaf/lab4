hexeditplus: hexeditplus.o 
	gcc -g -m32 -Wall -o hexeditplus hexeditplus.o 
hexeditplus.o: hexeditplus.c 
	gcc -m32 -g -Wall -c -o hexeditplus.o hexeditplus.c 

#-fno-pie' and '-fno-stack-protector
.PHONY: clean 
clean: 
	rm -f *.o hexeditplus