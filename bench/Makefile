SHELL=zsh

CPPFLAGS+=-Wall -Wextra -I../include -std=c++14
CFLAGS+=-ggdb -O3 -march=native
LIBS+=-lbenchmark

all: dna4_revcomp hash


%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

dna4_revcomp: dna4_revcomp.o ../libdna.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS)

hash: hash.o ../libdna.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	setopt null_glob; $(RM) -rf *.o
	$(RM) -f dna4_revcomp
