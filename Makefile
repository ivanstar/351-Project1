all: sender recv

sender: sender.o
	g++ sender.o -o sender

recv: recv.o
	g++ recv.o -o recv

clean:
	rm *.o sender recv
