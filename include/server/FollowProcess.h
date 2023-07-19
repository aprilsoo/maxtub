/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 14:57:59
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-19 13:17:32
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
#include "server/AcceptLock.h"
#include "server/Timer.h"
#include "ClientData.h"

using namespace std;


class FollowProcess{
  private:
    //监听套接字
    static int listen_fd;
    //锁
    static AcceptLock * lck;
    //编号
    static int id;
    //触发方式 ET/LT
    static int trigger;
    //闲置连接时间限制
    static long long time_limit;
    //定时器
    static Timer * timer;

    static EpollControl* ep;

    static once_flag of;

    static FollowProcess* instance;
    
    static unordered_map<int,std::unique_ptr<ClientData> > clients;

    FollowProcess(){}
    ~FollowProcess(){}
  public:
    
    static FollowProcess * Instance(){
      call_once(of,[&]{
        instance = new FollowProcess();
      });
      return instance;
    }
    
    static pid_t create_follow_process(
      AcceptLock *lck_,
      int listen_fd_,
      int id_,
      long long time_limit_,
      int trigger_
    ){
      lck = lck_;
      listen_fd = listen_fd_;
      id = id_;
      time_limit = time_limit_;
      trigger = trigger_;

      int pid = fork();
      if(pid == -1){
        LOG_ERROR("进程%d创建失败",id);
        return -1;
      }else if(pid > 0){
        LOG_INFO("进程%d创建成功 pid = %d",id,pid);
        return pid;
      }
      
      
      follow_process_start();

      LOG_INFO("进程%d pid=%d 结束",id,getpid());
      exit(0);
      return 0;
    }



    static void follow_process_start(){
      //创建ep 最大返回数量10000
      ep = new EpollControl(10000,EPOLLET);
      struct epoll_event * evs;

      //创建定时器
      timer = new Timer(ep,time_limit);
      
      for(;;){
        if(lck->lock()){
          add_client();
          lck->unlock();
        }

        int num = ep->wait(evs,100);
        
        for(int i=0;i<num;++i){
          int timer_fd = timer->find(evs[i].data.fd);
          if(timer_fd != -1){
            LOG_DEBUG("定时器到时 fd = %d timerfd = %d",evs[i].data.fd,timer_fd);
            deal_close(evs[i].data.fd);
            continue;
          }

          if(!clients.count(evs[i].data.fd)){
            continue;
          }
          
          if(evs[i].events & EPOLLIN){
            deal_read(evs[i].data.fd);
          }
          else if(evs[i].events & EPOLLOUT){
            deal_send(evs[i].data.fd);
          }
          else if(evs[i].events & EPOLLHUP || evs[i].events & EPOLLRDHUP || evs[i].events  & EPOLLERR){
            deal_close(evs[i].data.fd);
          }
          
        }
      }
    }

    static void add_client(){
      struct sockaddr_in addr;
      socklen_t len = sizeof(addr);
      for(;;){
        int ret = accept(listen_fd,(struct sockaddr*)&addr,&len);
        if(ret == -1){
          break;
        }
        clients[ret] = make_unique<ClientData>(ret,addr,ClientData::Status::ONCE_CONNECT);
        ep->add(ret,EPOLLIN);
        
        // if(ret > 1024){
        //   LOG_ERROR("fd = %d ",ret);
        // }
        LOG_INFO("建立新连接pid = %d fd = %d",getpid(),ret);

        timer->add_timer(ret);
      }
    }

    static void deal_read(int fd){
      LOG_DEBUG("fd = %d,deal_read",fd);
      timer->update_timer(fd);
      int ret = clients[fd] -> client_read();
      if(ret == -1){
        deal_close(fd);
      }
      else{
        if(!HttpAnalysis()){
          LOG_ERROR("解析失败,fd = %d",fd);
          return;
        }
        
        if(ep->mod(fd,EPOLLOUT) == -1){
          LOG_ERROR("add epollout事件失败 %s",strerror(errno));
        }
      }
    }

    /// @brief 解析
    /// @return 成功1 失败0
    static bool HttpAnalysis(){
      int t =1000;
      while(t--){
      }
      return true;
    }
    
    static void deal_send(int fd){
      LOG_DEBUG("fd = %d,deal_send",fd);
      int ret = clients[fd] -> client_write();
      if(ret == -1){
        deal_close(fd);
      }
      else{
        ep->add(fd,EPOLLIN);
      }
    }

    static void deal_close(int fd){
      LOG_DEBUG("fd = %d,deal_close",fd);
      timer->del_timer(fd);
      clients.erase(fd);
      close(fd);
    }
};

FollowProcess* FollowProcess::instance = nullptr;
once_flag FollowProcess::of;

int FollowProcess::listen_fd = -1;
AcceptLock* FollowProcess::lck = nullptr;
int FollowProcess::id = -1;
int FollowProcess::trigger = -1;
long long FollowProcess::time_limit = -1;
Timer* FollowProcess::timer = nullptr;
EpollControl* FollowProcess::ep = nullptr;
unordered_map<int,std::unique_ptr<ClientData> > FollowProcess::clients;

