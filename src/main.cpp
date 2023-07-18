/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 15:21:50
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-18 14:34:31
 * @FilePath: /maxtub/src/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "logger/Logger.h"
#include "server/maxtub.h"

//后续执行命令:xx.exe ./conf/shared_bike.ini
int main(int argc,char** argv)
{
    if(!Logger::instance()->init(std::string("../src/conf/logger.conf")))
    {
        //初始化日志模块失败
        fprintf(stderr,"init log module failed.\n");
        return -2;
    }
    LOG_DEBUG("日志打开");

    auto m = new maxtub();
    if(m->open()){
        LOG_DEBUG("打开成功");
    }else{
        LOG_DEBUG("打开失败");
    }

    return 0;
}
