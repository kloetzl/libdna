SHELL=zsh

CPPFLAGS+=-Wall -Wextra $(shell pkg-config --cflags-only-I glib-2.0) -I../include
CFLAGS+=-ggdb -Og -std=gnu11 -fsanitize=address
LIBS+=-lglib-2.0 -lm

.PHONY: clean format

TESTSF=dna4_revcomp.c dna4_evodist_jc.c translate.c
TESTS=$(TESTSF:.c=)

all: $(TESTS)

dna%: dna%.o ../libdna.a util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

translate: translate.o ../libdna.a util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

format:
	clang-format -i *.h *.c

clean:
	setopt null_glob; $(RM) -rf *.o *.a
	$(RM) -f $(TESTS)
