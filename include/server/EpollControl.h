/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-14 13:34:46
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-14 14:27:11
 * @FilePath: /maxtub/include/server/EpollControl.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AEpragmatic
 */

#pragma once

#include<sys/epoll.h>
#include"logger/Logger.h"
#include<unistd.h>
#include<memory.h>
#include<stdlib.h>

class EpollControl{
  private:
    int epoll_fd;
    double time_limit;
    int evs_num;
    struct epoll_event ev,*evs;

  public:
    enum OP{
      add = EPOLL_CTL_ADD,
      del = EPOLL_CTL_DEL,
      mod = EPOLL_CTL_MOD
    };

    EpollControl(
      double time_limit_,
      int evs_num_
    ):
    time_limit(time_limit_),
    evs_num(evs_num_)
    {
      epoll_fd = epoll_create(1);
      evs = (struct epoll_event*)malloc(sizeof(struct epoll_event) * evs_num);
    }

    ~EpollControl(){
      if(epoll_fd != 1){
        close(epoll_fd);
      }
      if(evs!=nullptr){
        delete(evs);
      }
    }


    /// @brief 修改事件
    /// @param events 事件类型
    /// @param fd 
    /// @param op OP
    /// @return 返回结果
    int epoll_control(uint32_t events,int fd,int op){
      ev.events = events;
      ev.data.fd = fd;
      return epoll_ctl(epoll_fd,op,fd,&ev);
    }
    
    int epoll_wait(std::vector<epoll_event>& activeEvents,int timeout=10){

    }

    int GetEventFd(int i){

    }

    int GetEvent(int i){

    }

    
};