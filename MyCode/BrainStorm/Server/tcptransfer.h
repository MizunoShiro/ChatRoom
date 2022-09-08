#ifndef TCPTRANSFER_H
#define TCPTRANSFER_H

#include <fstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <spdlog/spdlog.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <cstdio>
#define PORT 6667
#define DEBUG

class TcpTransfer
{
public:
    TcpTransfer();
public:
    //创建服务器
    int serverCreate();
    
    void initNet();

    //处理用户
    static void* ThreadStart(void *fd);
private:
    std::shared_ptr<spdlog::logger> _log;   //记录日志的句柄

    static pthread_mutex_t m_mutex;
};


#endif // TCPTRANSFER_H
