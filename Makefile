CFLAGS=-o build/okiz
LDFLAGS=-Oz
LDLIBS=-larchive

build:
	mkdir build
	$(CC) $(LDFLAGS) src/okiz.c $(LDLIBS) $(CFLAGS)

docs:
	mkdir -p build/docs
	pandoc -s -t man docs/okiz.md -o build/docs/okiz.1

clean:
	rm -rf build
