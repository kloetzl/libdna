SHELL=zsh

VPATH+=src

CPPFLAGS+=-Wall -Wextra -Iinclude -Isrc
CFLAGS+=-ggdb -O2 -std=gnu11 -fPIC
LIBS+=


.PHONY: clean format all install

all: libdna.a libdna.so

RELEASE: CPPFLAGS+=-DNDEBUG
RELEASE: libdna.a libdna.so

libdna.a: $(OBJECTS)
	$(AR) -qs $@ $^

libdna.so: $(OBJECTS)
	$(CC) -shared -o $@ $^

install: libdna.so
	# not sure if this works
	install -t $(PREFIX)/usr/include include/dna.h
	install -t $(PREFIX)/usr/lib libdna.so
	install -t $(PREFIX)/usr/share/man/man7 man/*.7

format:
	clang-format -i src/*.h src/*.c include/*.h

clean:
	setopt null_glob; $(RM) -rf *.o dna_test *.a *.so
