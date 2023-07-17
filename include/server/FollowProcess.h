/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 14:57:59
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-17 14:56:07
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
#include "HttpConn.h"
#include "server/AcceptLock.h"

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
    static double time_limit;

    std::unique_ptr<EpollControl> ep;

    FollowProcess(){}
    ~FollowProcess(){}

    static once_flag of;

    static FollowProcess* instance;
    
    unordered_map<int,std::unique_ptr<ClientData> > clients;
  public:
    
    static FollowProcess * Instance(){
      call_once(of,[&]{
        instance = new FollowProcess();
      });
      return instance;
    }
    
    pid_t create_follow_process(
      AcceptLock *lck_,
      int listen_fd_,
      int id_,
      double time_limit_,
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



    void follow_process_start(){
      //创建ep 最大返回数量10000
      ep.reset(new EpollControl(10000));

      struct epoll_event * evs;

      for(;;){
        if(lck->lock()){
          add_client();
          lck->unlock();
        }

        int num = ep->wait(evs,100);
        for(int i=0;i<num;++i){
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

    void add_client(){
      struct sockaddr_in addr;
      socklen_t len = sizeof(addr);
      for(;;){
        int ret = accept(listen_fd,(struct sockaddr*)&addr,&len);
        if(ret == -1){
          break;
        }
        clients[ret] = make_unique<ClientData>(ret,addr);
        ep->add(ret,EPOLLIN);
      }
    }

    void deal_read(int fd){
      int ret = clients[fd] -> read();
      if(ret == -1){
        deal_close(fd);
      }
      else{
        ep->add(fd,EPOLLOUT);
      }
    }
    
    void deal_send(int fd){
      int ret = clients[fd] -> send();
      if(ret == -1){
        deal_close(fd);
      }
      else{
        ep->add(fd,EPOLLIN);
      }
    }

    void deal_close(int fd){
      
      clients.erase(fd);
      close(fd);
    }
};

static FollowProcess* instance = nullptr;
static once_flag of;