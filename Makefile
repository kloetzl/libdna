VERSION=0.1
SOVERSION=1

DESTDIR?=/usr
LIBDIR?=$(DESTDIR)/lib
INCLUDEDIR?=$(DESTDIR)/include
MANDIR?=$(DESTDIR)/share/man

SHELL=zsh
SONAME=libdna.so.$(SOVERSION)

UNAME_S=$(shell uname -s)
ifeq ($(UNAME_S),Linux)
	DYNAMIC_FLAG=-Wl,-soname,$(SONAME)
endif
ifeq ($(UNAME_S),Darwin)
	DYNAMIC_FLAG=-dynamic
# 	CFLAGS_MACOS=-Wl,-U,_strtonum
endif

export CC
export CFLAGS
export CPPFLAGS
export CXX
export CXXFLAGS


.PHONY: all clean format install-dev install-lib install

all: libdna.a $(SONAME)

$(SONAME): libdna.a
	$(CC) $(DYNAMIC_FLAG) -shared  -o $@ $^

src/libdna.a:
	$(MAKE) -C $(@D) $(@F)

libdna.a: src/libdna.a
	cp $^ $@

install-dev:
	install -D -m0644 -t "$(INCLUDEDIR)" include/dna.h

install-lib: $(SONAME)
	install -D -t "$(PREFIX)/usr/lib" $(SONAME)
	install -D -t "$(PREFIX)/usr/lib" libdna.a
	install -D -t "$(PREFIX)/usr/share/man/man7" man/*.7
	# does the following work on macOS?
	ln -f -r -s "$(LIBDIR)/$(SONAME)" "$(LIBDIR)/libdna.so"

install: install-dev install-lib

format:
	$(MAKE) -C src format
	$(MAKE) -C bench format
	clang-format -i src/*.h src/*.c include/*.h

clean:
	$(MAKE) -C src clean
	$(MAKE) -C bench clean
	setopt null_glob; $(RM) -rf *.o dna_test *.a *.so
