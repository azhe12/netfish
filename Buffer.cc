//azhe liuyuanzhe123@126.com

#include <errno.h>
#include <memory.h>
#include <sys/uio.h>

#include "Buffer.h"
#include "SocketsOps.h"
#include "Logging.h"

using namespace netfish;

//使用readv，并且利用stack 空间来读取
//一次最多可读取65536+writable bytes
ssize_t Buffer::readFd(int fd, int* savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const ssize_t n = readv(fd, vec, 2);
    if (n < 0) {
        *savedErrno = errno;
    } else if (n <= writable) {
        writerIndex_ += n;
    } else {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    return n;
}
