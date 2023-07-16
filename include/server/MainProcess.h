/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 14:57:50
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-16 19:31:28
 * @FilePath: /maxtub/include/server/MainProcess.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include"server/FollowProcess.h"
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
#include <semaphore.h>

#include"logger/Logger.h"

using namespace std;



class MainProcess{
  private:
  //工作进程节点
    struct Node{
      int id;
      int pid;
    };
    
    //监听套接字
    int socket_fd; 
    //进程数
    int process_num;
    //触发方式 ET/LT
    int trigger;
    //协议IPV4/IPV6
    int socket_family;
    //端口
    int port;
    //TCP/UDP
    int protocol;
    //ip地址
    char *ip;
    //闲置连接时间限制
    double time_limit;
    
    //锁
    AcceptLock * lck;

    //工作线程管理
    list<MainProcess::Node> follows;
    
  public:
    enum TRIGGER{
      ET=0,
      LT
    };
    enum FAMILY{
      IPV4=AF_INET,
      IPV6=AF_INET6
    };
    enum PROTOCOL{
      TCP=SOCK_STREAM,
      UDP=SOCK_DGRAM
    };
    
    MainProcess(
      int process_num_,
      int trigger_,
      int socket_family_,
      int port_,
      int protocol_,
      char *ip_
    ):
    process_num(process_num_),
    trigger(trigger_),
    socket_family(socket_family_),
    port(port_),
    protocol(protocol_),
    ip(ip_)
    {
      path_ = getcwd(nullptr, 256);
      assert(path_);
    }
    
    /// @brief 
    /// @return 成功返回1 失败返回-1 
    int start(){
      if(init_socket_fd() < 0){
        return -1;
      }
      
      if(init_bind() < 0){
        close(socket_fd);
        return -1;
      }

      if(init_listen() < 0){
        close(socket_fd);
        return -1;
      }

      if(create_follows() < 0){
        close(socket_fd);
        return -1;
      }

      for(;;){
        wait_sign();
      }
    }

    /// @brief 创建socket_fd
    /// @return 成功返回socket_fd 失败返回-1
    int init_socket_fd(){
      socket_fd = socket(socket_family,protocol,0);
      if(socket_fd < 0){
        LOG_ERROR("初始化sockfd失败");
        return -1;
      }

      int flags = fcntl(socket_fd,F_GETFL,0);
      fcntl(socket_fd,F_SETFL,flags|O_NONBLOCK);

      int opt = 1;
      setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

      return socket_fd;
    }

    /// @brief bind
    /// @return 成功返回1 失败返回-1 
    int init_bind(){
      struct sockaddr_in sockaddr;
      memset(&sockaddr,0,sizeof(sockaddr));
      sockaddr.sin_port = htons(port);
      sockaddr.sin_family = socket_family;
      int ok = inet_pton(socket_family,ip,&sockaddr.sin_addr.s_addr);
      if(ok != 1){
        LOG_ERROR("IP地址绑定失败");
        return -1;
      }

      ok = bind(socket_fd,(struct sockaddr*)&sockaddr,sizeof(sockaddr));
      if(ok < 0){
        LOG_ERROR("bind失败");
        return -1;
      }

      return 1;
    }

    /// @brief listen
    /// @return 成功返回1 失败返回-1
    int init_listen(){
      int ok = listen(socket_fd,500);
      if(ok < 0){
        return -1;
      }
      return 1;
    }


    /// @brief 创建工作进程
    /// @return 失败返回-1 成功返回创建的进程数
    int create_follows(){
      for(int i=0;i<process_num;++i){
        int pid = FollowProcess::Instance()->create_follow_process(lck,socket_fd,i,time_limit,trigger);
        if(pid == -1){
          
        }else{
          follows.push_back({i,pid});
        }
      }
      return follows.size();
    }

    
    /// @brief 接受信号
    void wait_sign(){

    }

};