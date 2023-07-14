/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 14:57:50
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-14 14:39:41
 * @FilePath: /maxtub/include/server/MainProcess.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include<bits/stdc++.h>
#include"server/FollowProcess.h"
#include<sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include"logger/Logger.h"

using namespace std;



class MainProcess{
  private:
    int socket_fd;
    int process_num;
    int trigger;
    int socket_family;
    int port;
    int protocol;
    char *ip;
    struct sockaddr_in sockaddr;
    vector<pair<int,int*> > addrs;
    list<FollowProcess*> follows;

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
    {}
    
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

      for(int i=0;i<process_num;i++){
        int *pos = new int;
        addrs.push_back({i,pos});
        FollowProcess.Instance();
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


};