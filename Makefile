LIB_SRC= \
		Thread.cc 		\
		Timestamp.cc 	\
		Channel.cc 		\
		EventLoop.cc 	\
		EPoller.cc

BINARIES=testThread

all:$(BINARIES)

include netfish.mk

testThread:testThread.cc

clean:
	rm -rf $(BINARIES) a.out *.o
