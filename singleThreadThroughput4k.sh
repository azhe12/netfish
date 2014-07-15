#!/bin/sh
export NETFISH_LOG_LEVEL=4 

killall pingPongServer
timeout=${timeout:-100}
#bufsize=16384
bufsize=4096
 
for nosessions in 1 10 100 1000 10000; do
  for nothreads in 1; do
    sleep 5
    echo "Bufsize: $bufsize Threads: $nothreads Sessions: $nosessions"
    ./pingPongServer 0.0.0.0 55555 $nothreads $bufsize & srvpid=$!
    ./pingPongClient 127.0.0.1 55555 $nothreads $bufsize $nosessions $timeout
    kill -9 $srvpid
  done
done
