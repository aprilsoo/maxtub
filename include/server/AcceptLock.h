#pragma once

#include <bits/stdc++.h>
#include <mutex>
#include <assert.h>
#include <memory>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/file.h>
#include <semaphore.h>
#include"logger/Logger.h"

class AcceptLock{
  private: 
    //锁文件描述符
    int fd;
    //锁的时限
    int limit;
    //超时延时时间
    int extend_time;
    //尝试锁信号量 = 1
    sem_t *sem_try;
    //尝试创建信号量 = 1
    sem_t *sem_create; 
  public:

    AcceptLock(
      int limit_, 
      int extime,
      sem_t *sem_try_,
      sem_t *sem_create_,
    ):
    {
      limit = limit_;
      sem_try = sem_try_;
      sem_create = sem_create_;
      extend_time = extime;
    }

    int create(){
      sem_wait(sem_create);
      fd = open("lockfile.txt",O_RDWR | O_CREAT, 0644);
      sem_post(sem_create);
      if(fd == -1){
        LOG_ERROR("锁文件创建失败");
      }
    }

    int lock(){
      sem_wait(sem_try);
      int ret = flock(fd, LOCK_EX | LOCK_NB);
      sem_post(sem_try);
      if(ret == -1) return false;
      
      return true;
    }
    
    void unlock(){
      flock(fd, LOCK_UN);
    }
    

}