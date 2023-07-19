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

using namespace std;

class ListenLock{
  private:
    struct Node{
      pthread_mutex_t mutex;
      pthread_mutexattr_t mutexattr;
    };
    Node *nd;
  public:
    ListenLock(){
      nd = (Node*)mmap(nullptr,sizeof(Node),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
      pthread_mutexattr_init(&nd->mutexattr);
      pthread_mutexattr_setpshared(&nd->mutexattr, PTHREAD_PROCESS_SHARED);
      pthread_mutex_init(&nd->mutex,&nd->mutexattr);
    }

    int lock(){
      int err = pthread_mutex_trylock(&nd->mutex);
      if(err != 0){
        return -1;
      }
      return 1;
    }

    void unlock(){
      pthread_mutex_unlock(&nd->mutex);
    }
};