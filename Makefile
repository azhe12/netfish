LIB_SRC= \
		Thread.cc 		\
		Timestamp.cc 	\
		Channel.cc 		\
		EventLoop.cc 	\
		EPoller.cc		\
		InetAddress.cc	\
		Socket.cc		\
		SocketsOps.cc	\
		Acceptor.cc

BINARIES=	\
			testThread		\
			testEventLoop	\
			testAcceptor	\
			testChannel

all:$(BINARIES)

include netfish.mk

testThread:testThread.cc
testEventLoop:testEventLoop.cc
testAcceptor:testAcceptor.cc
testChannel:testChannel.cc

clean:
	rm -rf $(BINARIES) a.out *.o
