/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-12 15:24:48
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-18 14:28:27
 * @FilePath: /maxtub/src/logger/Logger.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "logger/Logger.h"

#include <iostream>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RemoteSyslogAppender.hh>
#include <log4cpp/PropertyConfigurator.hh>

//外部的static成员必须声明一下
Logger Logger::instance_;

bool  Logger::init(const std::string& log_conf_file)
{
	try{
		//属性设置
		log4cpp::PropertyConfigurator::configure(log_conf_file);		
	}catch(log4cpp::ConfigureFailure& f)
	{
		//如果出现异常
		std::cerr << " load log config file " << log_conf_file.c_str() << " failed with result : " << f.what()<< std::endl;
        return false;

	}
	//log4cpp::Category* category_;
	category_ = &log4cpp::Category::getRoot();
	return true;
}
