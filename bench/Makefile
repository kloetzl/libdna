SHELL=zsh

CPPFLAGS+=-Wall -Wextra -I../include -std=c++14 -DNDEBUG
CXXFLAGS+=-ggdb -Ofast -march=native -ftree-vectorize #-ftree-loop-vectorize
LIBS+=-lbenchmark

BINS= \
	compare \
	compare_rev \
	count \
	mismatch \
	pack \
	replace \
	revcomp

.PHONY: all format clean

all: $(BINS)


%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

$(BINS): %: %.o common.o ../src/libdna.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS)

format:
	setopt null_glob; clang-format -i *.h *.c *.cxx

clean:
	setopt null_glob; $(RM) -rf *.o perf.data*
	$(RM) -f $(BINS) common
