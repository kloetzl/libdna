SHELL=zsh

CPPFLAGS+=-Wall -Wextra -I../include -std=c++14
CXXFLAGS+=-ggdb -O3 #-march=native -ftree-vectorize
LIBS+=-lbenchmark

BINS=revcomp hash count

all: $(BINS)


%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

revcomp: revcomp.o ../libdna.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS)

hash: hash.o ../libdna.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS)

count: count.o ../libdna.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	setopt null_glob; $(RM) -rf *.o perf.data*
	$(RM) -f $(BINS)
