#include<sys/types.h>

class maxtub{
  private:
  char * IP;
  char * PORT;
  size_t PROCESS_NUM;
  size_t LOG_LEVEL;
  double TIME_LIMIT;

  public:

    maxtub(char * IP_,
    char * PORT_,
    size_t PROCESS_NUM_,
    double TIME_LIMIT_,
    size_t LOG_LEVEL_
    ):IP(IP_),PORT(PORT_),PROCESS_NUM(PROCESS_NUM_),TIME_LIMIT(TIME_LIMIT_),LOG_LEVEL(LOG_LEVEL_)  {}

    int start(){
      
    }
};