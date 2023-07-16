/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 14:57:59
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-14 15:04:24
 * @FilePath: /maxtub/include/server/FollowProcess.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include <bits/stdc++.h>
#include <mutex>
#include <assert.h>
#include <memory>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/file.h>
#include <ctime>
#include "EpollControl.h"

class FollowProcess{
  private:
    int listen_fd;
    int* pos = nullptr;
    bool isClose_;
    int id_;
    int lock_fd_;

    std::unique_ptr<EpollControl> epoller_;
    std::vector<epoll_event> aevent_;
  public:

    FollowProcess(
      int sock_fd,
      int id,
      int lock_fd
      ):
      epoller_(new EpollControl(20,30)),
      listen_fd(sock_fd), pos(nullptr), 
      id_(id), isClose_(false),lock_fd_(lock_fd)
    {
    }
    ~FollowProcess(){}

    void follow_process_start(){
      uint32_t events = EPOLLIN;
      epoller_->epoll_control(events,listen_fd,EPOLL_CTL_ADD);  

      while(!isClose_){
        //尝试加锁
        int ret = flock(lock_fd_, LOCK_EX | LOCK_NB);
        int nums = epoller_->epoll_wait(aevent_);

        //拿到锁
        if(ret != -1){        
          for(int i = 0;i < nums;i++){
            int fd = epoller_->GetEventFd(i);
            if(fd == listen_fd){
              deal_listen_();
            }
          }
          flock(lock_fd_, LOCK_UN);
        } else { // 未拿到锁
           for(int i = 0;i < nums;i++){
            int fd = epoller_->GetEventFd(i);
            uint32_t events = epoller_->GetEvent(i);
            if(fd == listen_fd){
              continue;
            }else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                assert(users_.count(fd) > 0);
                close_conn_(&users_[fd]);
            }
            else if(events & EPOLLIN) {
                assert(users_.count(fd) > 0);
                deal_read_(&users_[fd]);
            }
            else if(events & EPOLLOUT) {
                assert(users_.count(fd) > 0);
                deal_write_(&users_[fd]);
            } else {
                LOG_ERROR("Unexpected event");
            }
          }         
        }

      }

    }

    void deal_listen_(){

    }

    void close_conn_(){

    }

    void deal_read_(){

    }

    void deal_write_(){

    }
};