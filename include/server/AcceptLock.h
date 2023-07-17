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
    //尝试锁信号量 = 1
    sem_t *sem_try;

    int create(){
      fd = open("lockfile.txt",O_RDWR | O_CREAT, 0644);
      if(fd == -1){
        LOG_ERROR("锁文件创建失败");
      }
    }

  public:

    AcceptLock()
    {
      sem_try = new sem_t();
      sem_init(sem_try, 1, 1);
      create();
    }

    /// @brief 尝试加锁
    /// @return 抢到锁返回true，未抢到返回false
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
    

};