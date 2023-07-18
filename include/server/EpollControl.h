/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-14 13:34:46
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-18 15:10:38
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
    int evs_num;
    struct epoll_event ev,*evs;
    int set_events;
  public:
      EpollControl(
      int evs_num_,
      int events_
    ):
    evs_num(evs_num_),
    set_events(events_)
    {
      set_events |= EPOLLONESHOT;
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

    /// @brief 
    /// @param fd 描述符
    /// @param events 事件类型
    /// @return 结果
    int add(int fd,uint32_t events){
      events |= set_events;
      ev.events = events;
      ev.data.fd = fd;
      return epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&ev);
    }

    /// @brief 
    /// @param fd 描述符
    /// @param events 事件类型
    /// @return 结果
    int del(int fd,uint32_t events){
      events |= set_events;
      ev.events = events;
      ev.data.fd = fd;
      return epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,&ev);
    }

    /// @brief 
    /// @param fd 描述符
    /// @param events 事件类型
    /// @return 结果
    int mod(int fd,uint32_t events){
      events |= set_events;
      ev.events = events;
      ev.data.fd = fd;
      return epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&ev);
    }

    /// @brief epoll_wait
    /// @param evs_ 存evs的地址
    /// @param wait_time_limit wait返回的时间限制
    /// @return 
    int wait(struct epoll_event* &evs_,int wait_time_limit){
      int num = epoll_wait(epoll_fd,evs,evs_num,wait_time_limit);
      evs_ = evs;
      return num;
    }

    

    
};