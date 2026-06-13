CFLAGS=-Os -o build/okiz
LDFLAGS=
LDLIBS=-larchive

build:
	mkdir build
	$(CC) $(LDFLAGS) src/okiz.c $(LDLIBS) $(CFLAGS)

install:
	sudo mkdir /etc/okiz
	sudo cp build/okiz /bin/
	sudo mkdir /usr/share/man/okiz
	sudo cp /build/docs/okiz.1 /usr/share/man/okiz/

docs:
	mkdir -p build/docs
	pandoc -s -t man docs/okiz.md -o build/docs/okiz.1

clean:
	rm -rf build
