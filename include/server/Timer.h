/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-17 15:17:05
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-19 13:15:36
 * @FilePath: /maxtub/include/server/Timmer.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include<bits/stdc++.h>
#include"server/EpollControl.h"
#include <sys/timerfd.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

class Timer{
  private:
    unordered_map<int,int> mp,rmp;
    struct itimerspec itimer;
    struct itimerspec old_itimer;
    EpollControl *ep;
    long long time_limit;
  public:
    /// @brief 
    /// @param ep_ 
    /// @param time_limit_ 纳秒
    Timer(
      EpollControl * ep_,
      long long time_limit_
    ):
    ep(ep_),
    time_limit(time_limit_){}

    /// @brief 
    /// @param fd 添加绑定的连接描述符
    /// @param limit_t 纳秒
    /// @return 成功返回定时器连接描述符 失败返回-1
    int add_timer(int fd,long long limit_t){
      int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
      if(timer_fd == -1){
        LOG_ERROR("定时器创建失败");
        return -1;
      }
      LOG_DEBUG("定时器 fd = %d timerfd = %d time_limit = %lld",fd,timer_fd,limit_t);
      
      itimer.it_value.tv_sec = limit_t;
      itimer.it_value.tv_nsec = 0;

      itimer.it_interval.tv_sec = 0;
      itimer.it_interval.tv_nsec = 0;
      

      if(timerfd_settime(timer_fd,0,&itimer,nullptr) == -1){
        close(timer_fd);
        LOG_ERROR("定时器设定事件失败 %s",strerror(errno));
        return -1;
      }

      if(ep->add(timer_fd,EPOLLIN) == -1){
        close(timer_fd);
        LOG_ERROR("定时器添加epoll事件失败");
        return -1;
      }

      mp[timer_fd] = fd;
      rmp[fd] = timer_fd;

      return timer_fd;
    }

    /// @brief 
    /// @param fd 添加绑定的连接描述符
    /// @return 成功返回定时器连接描述符 失败返回-1
    int add_timer(int fd){
      return add_timer(fd,time_limit);
    }

    /// @brief 
    /// @param fd 传连接描述符
    void del_timer(int fd){
      int timer_fd = rmp[fd];
      LOG_DEBUG("定时器删除 fd = %d timerfd = %d",fd,timer_fd);
      ep->del(timer_fd,0);
      mp.erase(timer_fd);
      rmp.erase(fd);
    }

    /// @brief 
    /// @param fd 添加绑定的连接描述符
    /// @param limit_t 纳秒
    /// @return 成功返回定时器连接描述符 失败返回-1
    int update_timer(int fd,long long limit_t){
      if(!rmp.count(fd)){
        return -1;
      }
      
      int timer_fd = rmp[fd];
      LOG_DEBUG("定时器更新 fd = %d timerfd = %d",fd,timer_fd);
      itimer.it_value.tv_sec = 0;
      itimer.it_value.tv_nsec = limit_t;

      itimer.it_interval.tv_nsec = 0;
      itimer.it_interval.tv_sec = 0;

      if(timerfd_settime(timer_fd,0,&itimer,nullptr) == -1){
        close(timer_fd);
        LOG_ERROR("更新定时器设定事件失败");
      }
      
      ep->del(timer_fd,0);

      if(ep->add(timer_fd,EPOLLIN) == -1){
        close(timer_fd);
        LOG_ERROR("更新定时器添加epoll事件失败");
      }

      return timer_fd;
    }

    /// @brief 
    /// @param fd 添加绑定的连接描述符
    /// @return 成功返回定时器连接描述符 失败返回-1
    int update_timer(int fd){
      return update_timer(fd,time_limit);
    }

    
    /// @brief 
    /// @param fd 描述符
    /// @return 如果是定时器描述符 返回绑定的连接描述符 ，否则返回-1
    int find(int fd){
      if(mp.count(fd)){
        return mp[fd];
      }
      return -1;
    }

};