#pragma once

#include<sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <iostream>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <atomic>
#include <sys/uio.h>


class Buffer{
  private:
    std::vector<char> *buffer_;
    std::atomic<std::size_t> readPos_;
    std::atomic<std::size_t> writePos_;

  public:
    Buffer(
      int size = 1000
    )
    {

      buffer_ = new std::vector<char>(size);
      readPos_ = 0;
      writePos_ = 0;

    }

    void clear(){
      readPos_ = 0;
      writePos_ = 0;
    }

    int readable(){
      return writePos_ - readPos_;
    }

    int writeable(){
      return buffer_->size() - writePos_;
    }

    int readFd(int fd){
      struct iovec iov[2];
      char buf[65535];
      int write_able = writeable();
      iov[0].iov_base = buffer_ + writePos_;
      iov[0].iov_len = writeable();
      iov[1].iov_base = buf;
      iov[1].iov_len = sizeof(buf);

      int len = readv(fd,iov,2);
      if(len == -1) return -1;
      if(len > write_able){
        buffer_->insert(buffer_->end(),buf,buf+(len-write_able));
      }

      writePos_ += len;
      return len;
    }

    int writeFd(int fd){
      int len = write(fd,buffer_,readable());
      if(len == -1) return -1;
      readPos_ += len;
      return len;
    }

};