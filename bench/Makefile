SHELL=zsh

CPPFLAGS+=-Wall -Wextra -I../include -std=c++14
CXXFLAGS+=-ggdb -O3 -march=native -ftree-vectorize
LIBS+=-lbenchmark

BINS=revcomp hash count replace compare

all: $(BINS)


%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^


revcomp: revcomp.o ../libdna.a
hash: hash.o ../libdna.a
count: count.o ../libdna.a
replace: replace.o ../libdna.a
compare: compare.o ../libdna.a

$(BINS):
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS)


clean:
	setopt null_glob; $(RM) -rf *.o perf.data*
	$(RM) -f $(BINS)
