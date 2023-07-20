/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 15:21:50
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-20 13:53:23
 * @FilePath: /maxtub/src/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "logger/Logger.h"
#include "server/MainProcess.h"

using namespace std;

MainProcess * m;
void hot_deployment(int signum){
    m->hot_deployment();
}

int main(int argc,char** argv)
{
    signal(SIGUSR1,hot_deployment);
    m = new MainProcess(string("../config/maxtub.conf"),string("../config/log.conf"));
    if(m->start() == -1){
        cout<<"maxtub 启动失败";
    }
    
    int x;
    cin>>x;
    m->Close();

    return 0;
}
