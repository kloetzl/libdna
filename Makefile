SHELL=zsh

CPPFLAGS+=-Wall -Wextra -Iinclude -Isrc
CFLAGS+=-ggdb -O2 -std=gnu11 -fPIC
LIBS+=


.PHONY: clean format all install

all: libdna.a libdna.so

RELEASE: CPPFLAGS+=-DNDEBUG
RELEASE: libdna.a libdna.so

src/libdna.a:
	$(MAKE) -C src libdna.a

src/libdna.so:
	$(MAKE) -C src libdna.so


libdna.a: src/libdna.a
	cp $^ $@

libdna.so: src/libdna.so
	cp $^ $@

install: libdna.so
	# not sure if this works
	install -t $(PREFIX)/usr/include include/dna.h
	install -t $(PREFIX)/usr/lib libdna.so
	install -t $(PREFIX)/usr/share/man/man7 man/*.7

format:
	clang-format -i src/*.h src/*.c include/*.h

clean:
	$(MAKE) -C src clean
	setopt null_glob; $(RM) -rf *.o dna_test *.a *.so
