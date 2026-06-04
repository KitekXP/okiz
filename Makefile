CFLAGS=-o build/okiz
LDFLAGS=-Oz
LDLIBS=-larchive

build:
	mkdir build
	$(CC) $(LDFLAGS) src/okiz.c $(LDLIBS) $(CFLAGS)

clean:
	rm -rf build
