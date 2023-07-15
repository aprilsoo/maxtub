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

#include "EpollControl.h"

class FollowProcess{
  private:
    int listen_fd;
    int* pos = nullptr;
    bool isClose_;
    int id_;
    int *share_addr;
    std::unique_ptr<EpollControl> epoller_;
    std::vector<epoll_event> aevent_;
  public:

    FollowProcess(
      int sock_fd,
      int id,
      int *share_
      ):
      epoller_(new EpollControl(20,30)),
      listen_fd(sock_fd), pos(nullptr), 
      id_(id), isClose_(false),share_addr(share_)
    {
    }
    ~FollowProcess(){}

    void follow_process_start(){
      uint32_t events = EPOLLIN;
      epoller_->epoll_control(events,listen_fd,EPOLL_CTL_ADD);
      // 读写锁
      while(!isClose_){
        // 抢锁
        if(share_addr[0] == -1){

        }
        int clock = share_addr[0];
        int nums = epoller_->epoll_wait(aevent_);
        if(clock == id_){        
          for(int i = 0;i < nums;i++){
            int fd = epoller_->GetEventFd(i);
            if(fd == listen_fd){
              deal_listen_();
            }
          
          }
          share_addr[0] = -1;
        } else {
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