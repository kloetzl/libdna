PREFIX?=/usr/local
PKG_CONFIG_PATH?=$(PREFIX)/lib/pkgconfig
LD_LIBRARY_PATH?=$(PREFIX)/lib
LIBDNA_CFLAGS?=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs --cflags --define-variable=prefix=$(PREFIX) libdna)
CFLAGS+=$(LIBDNA_CFLAGS)
CXXFLAGS+=$(LIBDNA_CFLAGS) -std=c++17

.PHONY: all clean check test
all: RTversion RTrevcomp RTcxxversion

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

RTversion: RTversion.o
	$(CC) -o $@ $^ $(CFLAGS)

RTcxxversion: RTcxxversion.o
	$(CXX) -o $@ $^ $(CFLAGS)

RTrevcomp: RTrevcomp.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) *.o
	$(RM) RTversion RTrevcomp RTcxxversion

check: test
test: RTversion RTrevcomp RTcxxversion
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./RTversion
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./RTrevcomp
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./RTcxxversion
