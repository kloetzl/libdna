SHELL=zsh

CPPFLAGS+=-Wall -Wextra -I../include -std=c++14
CXXFLAGS+=-ggdb -Ofast -march=native -ftree-vectorize #-ftree-loop-vectorize
LIBS+=-lbenchmark

BINS=revcomp hash count replace compare compare_rev

.PHONY: all format clean

all: $(BINS)


%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^


revcomp: revcomp.o ../src/libdna.a
hash: hash.o ../src/libdna.a
count: count.o ../src/libdna.a
replace: replace.o ../src/libdna.a
compare: compare.o ../src/libdna.a
compare_rev: compare_rev.o ../src/libdna.a

$(BINS):
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS)

format:
	setopt null_glob; clang-format -i *.h *.c *.cxx

clean:
	setopt null_glob; $(RM) -rf *.o perf.data*
	$(RM) -f $(BINS)
