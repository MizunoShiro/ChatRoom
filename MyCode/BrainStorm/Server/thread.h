#ifndef THREAD_H
#define THREAD_H

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <cstring>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/event_compat.h>

class Thread
{
public:
    Thread();
    void start();  //创建进程
    event_base *getBase();  //获取事件集合
protected:
    static void *work(void *arg);   //线程的工作函数
    static void pipeRead(evutil_socket_t ,short,void *arg);
    void run();
private:
    pthread_t m_threadId;
    event_base *m_base;
    int m_pipeReadFd;   //读端
    int m_pipeWriteFd;  //写端
    event m_pipeEvent;

};

#endif // THREAD_H
