/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 14:57:59
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-14 15:04:24
 * @FilePath: /maxtub/include/server/FollowProcess.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include"server/MainProcess.h"
#include<bits/stdc++.h>
#include<mutex>
#include <memory>

class FollowProcess{
  private:
    static int socket_fd;
    static int* pos;
    static int id;
    static FollowProcess *instance;
    static once_flag FollowProcess_once_flag;

    FollowProcess(){}
    ~FollowProcess(){}
  public:
    
    static FollowProcess* Instance(){ 
      std::call_once(FollowProcess_once_flag,[&]{
        instance = new FollowProcess();
      });
      
      return instance;
    }
    
    static int follow_process_start(
      int id_,
      int * pos_,
      int socket_fd_
    ){
      int pid = fork();
      if(pid != 0){
          return pid;
      }
      run();
      pos = pos_;
      socket_fd = socket_fd_;
      id = id_;
    }

    static void run(){
      for(;;){
        // 抢锁
        // 注册到epoll
        // 优先级
        // for
        // 
      }
    }
};

static FollowProcess * instance = nullptr;
static int socket_fd = 0;
static int* pos = nullptr;
static int id = 0;