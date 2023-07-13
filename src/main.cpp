/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 15:21:50
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-12 15:24:01
 * @FilePath: /maxtub/src/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "../include/logger/Logger.h"

//后续执行命令:xx.exe ./conf/shared_bike.ini
int main(int argc,char** argv)
{
    double a;
    double b;
    if(argc!=3)
    {
        printf("Please input shbk <config file path> <log file config>!\n");
        return -1;
    }
    //使用argv[2]对日志进行配置
    if(!Logger::instance()->init(std::string(argv[2])))
    {
        //初始化日志模块失败
        fprintf(stderr,"init log module failed.\n");
        return -2;
    }
    std::cin>>a;
    std::cin>>b;
    if(b==0)
    {
        LOG_ERROR("b can not zero\n");
        return -1;
    }else
    {
        LOG_INFO("a/b is %lf\n",a/b);
    }
    return 0;
}
