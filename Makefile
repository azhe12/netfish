LIB_SRC=Thread.cc Timestamp.cc

BINARIES=testThread

all:$(BINARIES)

include netfish.mk

testThread:testThread.cc

clean:
	rm -rf $(BINARIES) a.out *.o
