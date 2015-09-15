CC=gcc
CFLAGS=-I.
DEPS=

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bcast: mkdir bcast.o
	$(CC) -o ./build/bcast ./bcast.o;

listener: mkdir listener.o
	$(CC) -o ./build/listener ./listener.o

clean:
	rm -Rf ./build;

mkdir:
		if [ ! -d "./build" ]; then \
			mkdir ./build; \
		fi
