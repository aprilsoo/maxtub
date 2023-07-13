/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 15:24:40
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-12 15:25:04
 * @FilePath: /maxtub/include/logger/Logger.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include <string>
#include <log4cpp/Category.hh>

class Logger
{
public:
	//传log的配置文件
	bool init(const std::string& log_conf_file);
    //返回单例
	static Logger* instance()
	{
		return &instance_;
	}
	
	log4cpp::Category* GetHandle()
	{
		return category_;
	}
	
	
protected:
	//单例
	static Logger instance_;
	//利用category输出日志
	log4cpp::Category* category_;
};

//进行宏的定义，后续方便输出
#define LOG_INFO  Logger::instance()->GetHandle()->info
#define LOG_DEBUG Logger::instance()->GetHandle()->debug
#define LOG_ERROR Logger::instance()->GetHandle()->error
#define LOG_WARN  Logger::instance()->GetHandle()->warn

