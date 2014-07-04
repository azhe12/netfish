LIB_SRC= \
		Thread.cc 		\
		Timestamp.cc 	\
		Channel.cc 		\
		EventLoop.cc 	\
		EPoller.cc		\
		InetAddress.cc	\
		Socket.cc		\
		SocketsOps.cc	\
		Acceptor.cc		\
		Timer.cc		\
		TimerQueue.cc	\
		Buffer.cc		\
		EventLoopThread.cc		\
		EventLoopThreadPool.cc	\
		TcpConnection.cc		\
		TcpServer.cc			\
		Connector.cc

BINARIES=	\
			testThread		\
			testEventLoop	\
			testAcceptor	\
			testChannel		\
			testTimerQueue	\
			testBuffer		\
			testEventLoopThreadPool	\
			testTcpServerDiscard	\
			testTcpConnectionSend	\
			testTcpServerChargen	\
			testConnector			\
			test12

all:$(BINARIES)

include netfish.mk

testThread:testThread.cc
testEventLoop:testEventLoop.cc
testAcceptor:testAcceptor.cc
testChannel:testChannel.cc
testTimerQueue:testTimerQueue.cc
testBuffer:testBuffer.cc
testEventLoopThreadPool:testEventLoopThreadPool.cc
testTcpServerDiscard:testTcpServerDiscard.cc
testTcpConnectionSend:testTcpConnectionSend.cc
testTcpServerChargen:testTcpServerChargen.cc
testConnector:testConnector.cc
test12:test12.cc

clean:
	rm -rf $(BINARIES) a.out *.o
