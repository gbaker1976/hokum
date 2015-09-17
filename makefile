CC = gcc
CFLAGS = -I.
DEPS = 'fcproto.c'

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bcast: mkdir bcast.o fcproto.o
	$(CC) -o ./build/bcast ./bcast.o ./fcproto.o;

listener: mkdir listener.o
	$(CC) -o ./build/listener ./listener.o

clean:
	rm -Rf ./build;

mkdir:
		if [ ! -d "./build" ]; then \
			mkdir ./build; \
		fi
