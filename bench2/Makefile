
SEQAN3=${DISS_PATH}/sources/seqan3-git

CPPFLAGS=-Wall -Wextra
CXXFLAGS=-march=native -O3

.PHONY: all clean format

all: revcomp

foo: foo.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lbenchmark

revcomp: revcomp.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lbenchmark -L../builddir -ldna

%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -DNDEBUG                         \
		-std=c++17 -fconcepts                                       \
		-fconcepts-diagnostics-depth=2 \
		-I       ${SEQAN3}/include                                  \
		-isystem ${SEQAN3}/submodules/range-v3/include              \
		-isystem ${SEQAN3}/submodules/sdsl-lite/include             \
		-isystem ${SEQAN3}/submodules/cereal/include                \
		-DSEQAN3_HAS_ZLIB=1 -DSEQAN3_HAS_BZIP2=1                    \
		-lz -lbz2 -lstdc++fs -pthread -c                            \
		-L../builddir -ldna -I../include \
		-o $@ \
	$^

format:
	clang-format -i *.cxx

clean:
	$(RM) revcomp *.o
	$(RM) perf*
