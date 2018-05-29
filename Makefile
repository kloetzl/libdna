SHELL=zsh

VPATH+=src

CPPFLAGS+=-Wall -Wextra -Iinclude -Isrc
CFLAGS+=-ggdb -O2 -std=gnu11
LIBS+=

OBJECTS=\
	dna4_evodist_jc.o \
	dna4_evodist_k80.o \
	dna4_gc_content.o \
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

.PHONY: clean format

all: libdna.a

# dna_test: dna_test.o dna.o libdna.a
# 	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

RELEASE: CPPFLAGS+=-DNDEBUG
RELEASE: libdna.a

libdna.a: $(OBJECTS)
	$(AR) -qs $@ $^

format:
	clang-format -i src/*.h src/*.c include/*.h

clean:
	setopt null_glob; $(RM) -rf *.o dna_test *.a
