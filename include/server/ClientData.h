/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-17 15:14:58
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-19 13:40:27
 * @FilePath: /maxtub/include/server/ClientData.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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
#include <netinet/in.h>

#include "logger/Logger.h"
#include "Buffer.h"

class ClientData{
  private:
    //缓冲
    Buffer * read_buf;
    Buffer * write_buf;

    char a[1005];
    int fd;
    struct sockaddr_in addr;
    char * ip;
    int port;
    bool isET;

  public:

    //状态
    enum Status{
      ONCE_CONNECT = 0,
      CONTNS_CONNET = 1
    }status;
    
    
    ClientData(
      int fd_,
      struct sockaddr_in addr_,
      Status status_
    )
    {
      fd = fd_;
      addr = addr_;
      status = status_;
    }

    /// @brief 读到缓冲区
    /// @return -1表示读取数据失败；>0时，表示读出的字节数；等于0时，表示无数据可读
    int client_read(){
      char ch[1005];
      int len = read(fd,ch,1005);
      LOG_INFO("%s",ch);
      return len;
      int tot = 0;
      do{
        int len = read_buf->readFd(fd);
        string str = "";
        for(auto now : *read_buf->buffer_){
          str += now;
        }
        LOG_INFO("%s",str.c_str());
        if(len == 0){
          return -1;
        }
        if(len == -1){
          break;
        }
        tot += len;
        
      }while(isET);
      
      
      return tot;
    }

    /// @brief 发送给客户端
    /// @return 剩余未写 = 0, 一次性连接写完 = -1， 常连接写完 = 0, 写入错误 = 0报错
    int client_write(){
      char buf[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 113\r\n\r\n<!DOCTYPE html><html><head><title>Welcome to Example.com</title></head><body><h1>Hello, World!</h1></body></html>" ;
      int ret = write(fd,buf,strlen(buf)*sizeof(char));
      return 1;
      do{
        int len = write_buf->writeFd(fd);
        if(len == 0){
          break;
        }
        if(len == -1){
          LOG_ERROR("写入错误,fd = %d",fd);
          break;
        }
      }while(!write_buf->empty());
      if(status == ONCE_CONNECT && write_buf->empty()){
        return -1;
      }
      return 0;
    }
};