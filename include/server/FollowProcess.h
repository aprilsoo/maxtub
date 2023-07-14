/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 14:57:59
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-14 14:45:49
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
    MainProcess * main_process;
    int socket_fd;
    FollowProcess(){}
    static FollowProcess *instance;
  public:
    
    static FollowProcess* Instance(){ 
      call_once(FollowProcess);
      
      return instance;
    }
    
    int follow_process_start(
      int id_
    ){
      int pid = fork();
      if(pid != 0){
          return pid;
      }
      run();
    }

    void run(){
      for(;;){
        
      }
    }
};

static FollowProcess * instance = nullptr;