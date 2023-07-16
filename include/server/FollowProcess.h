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
    int *share_addr;
    sem_t *sem_read;
    sem_t *sem_write;
    sem_t *sem_count;
    int process_num;

    std::unique_ptr<EpollControl> epoller_;
    std::vector<epoll_event> aevent_;
  public:

    FollowProcess(
      int sock_fd,
      int id,
      int shared_memory_id,
      sem_t *sem_read_,
      sem_t *sem_write_,
      sem_t *read_count_,
      int p_num
      ):
      epoller_(new EpollControl(20,30)),
      listen_fd(sock_fd), pos(nullptr), 
      id_(id), isClose_(false),sem_read(sem_read_),
      sem_write(sem_write_),sem_count(read_count_),process_num(p_num)
    {
      // 连接共享内存
      share_addr = static_cast<int*>(shmat(shared_memory_id, nullptr, 0));
      srand(time(NULL));
    }
    ~FollowProcess(){}

    void follow_process_start(){
      uint32_t events = EPOLLIN;
      epoller_->epoll_control(events,listen_fd,EPOLL_CTL_ADD);  

      while(!isClose_){
        //读操作
        read_wait();
        int choice_ = *share_addr;
        read_post();
        //
        int nums = epoller_->epoll_wait(aevent_);
        if(choice_ == id_){        
          for(int i = 0;i < nums;i++){
            int fd = epoller_->GetEventFd(i);
            if(fd == listen_fd){
              deal_listen_();
            }
          
          }
          sem_wait(sem_write);
          *share_addr = rand()%process_num;
          sem_post(sem_write);
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

    void read_wait(){
        // 读wait
        sem_wait(sem_read);
        int read_count;
        sem_getvalue(sem_count,&read_count);
        if(read_count == 0){
          sem_wait(sem_write);
        }
        sem_post(sem_count);
        sem_post(sem_read);
    }

    void read_post(){
        // 读post
        sem_wait(sem_read);
        sem_wait(sem_count);
        int read_count;
        sem_getvalue(sem_count,&read_count);
        if(read_count == 0){
          sem_post(sem_write);
        }
        sem_post(sem_read);
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