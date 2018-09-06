SHELL=zsh

CPPFLAGS+=-Wall -Wextra -I../include -Isrc
CFLAGS+=-ggdb -O2 -std=gnu11 -fPIC
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
	dnax_translate.o \
	utils.o

SSE2_OBJECTS=\
	dna4_evodist_jc_sse2.o

AVX2_OBJECTS=\
	dna4_evodist_jc_avx2.o

AVX512_OBJECTS=\
	dna4_evodist_jc_avx512.o


$(SSE2_OBJECTS): CFLAGS+=-msse2 -mpopcnt
$(AVX2_OBJECTS): CFLAGS+=-mavx2 -mpopcnt
$(AVX512_OBJECTS): CFLAGS+=-mavx512bw -mavx512vl -mpopcnt


.PHONY: clean format all

all: libdna.a libdna.so

RELEASE: CPPFLAGS+=-DNDEBUG
RELEASE: libdna.a libdna.so

libdna.a: $(OBJECTS) $(SSE2_OBJECTS) $(AVX2_OBJECTS) $(AVX512_OBJECTS)
	$(AR) -qs $@ $^

libdna.so: $(OBJECTS)
	$(CC) -shared -o $@ $^

format:
	clang-format -i *.h *.c

clean:
	setopt null_glob; $(RM) -rf *.o *.a *.so
