client:
	if [ ! -d "./build" ]; then \
	mkdir ./build; \
	fi

	gcc ./client.c -o ./build/client;

clean:
	rm -Rf ./build;
