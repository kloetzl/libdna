PREFIX?=/usr/local
PKG_CONFIG_PATH?=$(PREFIX)/lib/pkgconfig
LD_LIBRARY_PATH?=$(PREFIX)/lib
LIBDNA_CFLAGS?=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs --cflags libdna)
CFLAGS+=$(LIBDNA_CFLAGS)
CXXFLAGS+=$(LIBDNA_CFLAGS) -std=c++17

.PHONY: all clean check test
all: RTversion RTrevcomp RTcxxversion

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

RTversion: RTversion.o
	$(CC) $(CFLAGS) -o $@ $^

RTcxxversion: RTcxxversion.o
	$(CXX) $(CFLAGS) -o $@ $^

RTrevcomp: RTrevcomp.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) *.o
	$(RM) RTversion RTrevcomp RTcxxversion

check: test
test: RTversion RTrevcomp RTcxxversion
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./RTversion
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./RTrevcomp
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./RTcxxversion
