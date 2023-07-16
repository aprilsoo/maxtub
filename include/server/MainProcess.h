/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 14:57:50
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-14 15:05:21
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
    struct Node{
      int idx;
      int *pos;
      int pid;
    };
    
    int socket_fd;
    char* path_;
    int process_num;
    int trigger;
    int socket_family;
    int port;
    int protocol;
    char *ip;
    FollowProcess *follow_process_ptr_;
    // 共享内存大小
    const int shared_memory_size = sizeof(int);
    // 共享内存的key值
    const key_t shared_memory_key = 1234;
    
    struct sockaddr_in sockaddr;
    
    vector<MainProcess::Node> addrs;
    list<FollowProcess*> follows;
    vector<pid_t> pids;

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

      // 创建共享内存
      int shared_memory_id = shmget(shared_memory_key, shared_memory_size, IPC_CREAT | 0666);
      if (shared_memory_id == -1) {
          std::cerr << "Failed to create shared memory: " << strerror(errno) << std::endl;
          return 1;
      }

      // 连接共享内存
      void* shared_memory = shmat(shared_memory_id, nullptr, 0);
      if (shared_memory == (void*)-1) {
          std::cerr << "Failed to attach shared memory: " << strerror(errno) << std::endl;
          return 1;
      }

      // 初始化共享数据
      int* share_ = static_cast<int*>(shared_memory);
      *share_ = 0;


      // 创建并初始化信号量
      sem_t* sem_read = sem_open("/read_semaphare", O_CREAT | O_EXCL, 0644, 1);
      if (sem_read == SEM_FAILED) {
          std::cerr << "Failed to create semaphore: " << strerror(errno) << std::endl;
          return 1;
      }
      sem_t* sem_write = sem_open("/write_semaphare", O_CREAT | O_EXCL, 0644, 1);
      if (sem_read == SEM_FAILED) {
          std::cerr << "Failed to create semaphore: " << strerror(errno) << std::endl;
          return 1;
      }
      sem_t* read_count = sem_open("/read_count", O_CREAT | O_EXCL, 0644, 0);
      if (sem_read == SEM_FAILED) {
          std::cerr << "Failed to create semaphore: " << strerror(errno) << std::endl;
          return 1;
      }

      // 创建多进程
      pid_t pid = -1;
      int id = -1;
      for(int i=0;i<process_num;i++){
          id = i;
          pid = fork();
          if(pid <= 0) {
            break;
          }
          pids.push_back(pid);
      }

      if(pid < 0){
        perror("error");
      } else if(pid == 0) {
        //子进程
        follow_process_ptr_ = new FollowProcess(socket_fd, id, shared_memory_id, sem_read, sem_write, read_count,process_num);
        follow_process_ptr_->follow_process_start();
      } else if(pid > 0) {
        //父进程        
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