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
		TcpServer.cc

BINARIES=	\
			testThread		\
			testEventLoop	\
			testAcceptor	\
			testChannel		\
			testTimerQueue	\
			testBuffer		\
			testEventLoopThreadPool	\
			testTcpServerDiscard	\
			testTcpConnectionSend

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

clean:
	rm -rf $(BINARIES) a.out *.o
