CC = gcc
CFLAGS = -g

all: simulador monitor 

simulador: simulador.o trataficheiro.o
	$(CC) -g -o simulador simulador.o trataficheiro.o -lpthread

monitor: monitor.o
	$(CC) -g -o monitor monitor.o -lpthread 
	
clean:
	rm -f *.o *~ simulador monitor
