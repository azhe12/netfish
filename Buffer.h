//azhe liuyuanzhe123@126.com
#ifndef NETFISH_BUFFER_H
#define NETFISH_BUFFER_H
#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>

#include "copyable.h"

namespace netfish
{
//--------------------------------------------------------
//| prependable bytes | readable bytes | writable bytes |
//|                   |                |                 |
//--------------------------------------------------------
//|                   |                |                 |
//0     <=      readerIndex_  <=     writerIndex_  <=    size()
//
class Buffer: public copyable
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    Buffer()
        : buffer_(kCheapPrepend + kInitialSize),
          readerIndex_(kCheapPrepend),
          writerIndex_(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == kInitialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    size_t readableBytes()
    {
        return writerIndex_ - readerIndex_;
    }

    size_t writableBytes()
    {
        return buffer_.size() - writerIndex_;
    }

    size_t prependableBytes()
    {
        return readerIndex_;
    }

    void swap(Buffer& rhs)
    {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_, rhs.readerIndex_);
        std::swap(writerIndex_, rhs.writerIndex_);
    }

    //readerIndex_的memory地址
    const char* peek() const
    {
        return begin() + readerIndex_;
    }

    //read data and move readerIndex_
    //取走len长度数据
    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        readerIndex_ += len;
    }

    void retrieveUntil(const char* end)
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveAll()
    {
        retrieve(readableBytes());
    }

    std::string retrieveAsString()
    {
        std::string str(peek(), readableBytes());
        retrieveAll();
        return str;
    }

    void append(const std::string& str)
    {
        append(str.data(), str.size());
    }

    void append(const char* str, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(str, str + len, beginWrite());
        hasWritten(len);
    }

    void ensureWritableBytes(size_t len)
    {
        if (len > writableBytes()) {    //空间不足
            if (writableBytes() + prependableBytes() < len) {   //resize
                buffer_.resize(writerIndex_ + len);
            } else {    //将data move到前半端
                assert(kCheapPrepend < readerIndex_);
                size_t readable = readableBytes();
                std::copy(begin()+readerIndex_,
                          begin()+writerIndex_,
                          begin()+kCheapPrepend);
                readerIndex_ = kCheapPrepend;
                writerIndex_ = readerIndex_ + readable;
                assert(readable == readableBytes());
            }
        }
        assert(writableBytes() >= len);
    }

    //将data放置到前端
    void prepend(const void* data, size_t len)
    {
        assert(len <= prependableBytes());
        readerIndex_ -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d+len, begin()+readerIndex_);
    }

    //收缩buffer大小为kCheapPrepend+readableBytes()+reserve
    void shrink(size_t reserve)
    {
        std::vector<char> buf(kCheapPrepend+readableBytes()+reserve);
        std::copy(peek(), peek()+readableBytes(), buf.begin()+kCheapPrepend);
        //std::copy(peek(), beginWrite(), buf.begin()+kCheapPrepend);
        buf.swap(buffer_);
    }

    void hasWritten(size_t len)
    {
        writerIndex_ += len;
    }

    //writerIndex_指向的memory address
    char* beginWrite()
    {
        return &(*buffer_.begin()) + writerIndex_;
    }

    const char* beginWrite() const
    {
        return &(*buffer_.begin()) + writerIndex_;
    }

    //读取fd data to buffer
    //返回读取的字节数，保存errno到savedErrno
    ssize_t readFd(int fd, int* savedErrno);

private:
    //buffer data memory起始address
    char* begin()
    {
        return &(*buffer_.begin());
    }
    const char* begin() const
    {
        return &(*buffer_.begin());
    }


    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};
}
#endif  //NETFISH_BUFFER_H
