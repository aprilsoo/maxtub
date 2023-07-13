/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 14:49:38
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-13 13:48:57
 * @FilePath: /maxtub/include/server/maxtub.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include<sys/types.h>
#include"server/MainProcess.h"
#include"server/FollowProcess.h"


class maxtub{
  private:
  char * IP;
  char * PORT;
  size_t PROCESS_NUM;
  size_t LOG_LEVEL;
  double TIME_LIMIT;

  MainProcess *mainprocess;

  public:

    maxtub(char * IP_,
    char * PORT_,
    size_t PROCESS_NUM_,
    double TIME_LIMIT_,
    size_t LOG_LEVEL_
    ):IP(IP_),
    PORT(PORT_),
    PROCESS_NUM(PROCESS_NUM_),
    TIME_LIMIT(TIME_LIMIT_),
    LOG_LEVEL(LOG_LEVEL_
    ){}

    int open(){
      mainprocess = new MainProcess();
      mainprocess->start();
      
    }
};