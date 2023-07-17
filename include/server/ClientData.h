/*
 * @Author: peace901 443257245@qq.com
 * @Date: 2023-07-17 15:14:58
 * @LastEditors: peace901 443257245@qq.com
 * @LastEditTime: 2023-07-17 17:12:10
 * @FilePath: /maxtub/include/server/ClientData.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

class ClientData{
  private:
    //缓冲
    
    Buffer * read_buf;
    Buffer * send_buf;
    
    int fd;
    struct socketaddr_in addr;
    char * ip;
    int port;

  public:
    int read(){
      
    }

    int send(){
      
    }
};