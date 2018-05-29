SHELL=zsh

VPATH+=src

CPPFLAGS+=-Wall -Wextra -Iinclude -Isrc
CFLAGS+=-ggdb -O3 -std=gnu11 -fPIC
LIBS+=

OBJECTS=\
	dna4_evodist_jc.o \
	dna4_evodist_k80.o \
	dna4_gc_content.o \
	dna4_hash.o \
	dna4_revcomp.o \
	dna_version.o \
	dnax_count.o \
	dnax_find_first_not_of.o \
	dnax_find_first_of.o \
	dnax_hash.o \
	dnax_mismatch.o \
	dnax_replace.o \
	dnax_revcomp.o \
	dnax_to_dna4.o \
	utils.o

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
