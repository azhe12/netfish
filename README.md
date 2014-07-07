# netfish README
@(网络编程)

## Overview

netfish是一个reactor模式的网络库，支持多线程，高并发，目前可以完成`C10k`问题。

* 线程模型 `One Loop Per Thread` + `Thread pool`

* IO模型为 `non-blocking IO`+ `IO multiplexing`


## Install

依赖boost, 首先安装boost. Ubuntu下:

```
$sudo apt-get install libboost-dev libboost-test-dev
```

然后: 

```
cd netfish/
make
```

即可得到`libnetfish.a` 和 若干`test*`测试程序.

## Usage

直接link产生的`libnetfish.a`静态库即可，具体使用方法可参考 若干`test*`测试程序, 以及[pingPongServer.cc](https://github.com/azhe12/netfish/blob/master/pingPongServer.cc),  [pingPongClient.cc](https://github.com/azhe12/netfish/blob/master/pingPongClient.cc)

### Throughput Test

**测试方法:** 

1. `server` [pingPongServer.cc](https://github.com/azhe12/netfish/blob/master/pingPongServer.cc)端和`client`端 [pingPongClient.cc](https://github.com/azhe12/netfish/blob/master/pingPongClient.cc)分别实现`echo` 程序.  
2. 分别对`单线程`和`多线程`情况下，`不同的并发数` 测试其throughput.  测试脚本分别为[singleThreadThroughput.sh](https://github.com/azhe12/netfish/blob/master/singleThreadThroughput.sh), [multiThreadThroughput.sh](https://github.com/azhe12/netfish/blob/master/multiThreadThroughput.sh)


**硬件**: Intel CORE(TM) i7-3630QM @ 2.4G 四核八线程 


**OS**: Ubuntu 12.04 64bit


参考 [测试结果图表](https://github.com/azhe12/netfish/tree/master/throughputTest)
