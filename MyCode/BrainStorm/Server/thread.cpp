#include "thread.h"

Thread::Thread()
{
    m_base = event_base_new();
    if(!m_base)
    {
        printf("Couldn't create an event_base: exit\n");
        exit(1);
    }
    //创建管道
    int fd[2];

    if(pipe(fd) == -1)
    {
        perror("pipe error!");
        exit(1);
    }
    m_pipeReadFd = fd[0];
    m_pipeWriteFd = fd[1];
    //让管道事件监听管道的读端
    //如果监听到 管道的读端有数据可读
    event_set(&m_pipeEvent, m_pipeReadFd, EV_READ | EV_PERSIST, pipeRead, this);
    //将事件添加到m_base集合中
    event_base_set(m_base, &m_pipeEvent);
    //开启事件的监听
    event_add(&m_pipeEvent, 0);
}
void Thread::pipeRead(evutil_socket_t ,short,void *)
{
    
}
void Thread::start()
{
    pthread_create(&m_threadId, NULL, work, this);
    //线程分离
    pthread_detach(m_threadId);
}
void* Thread::work(void *arg)
{
    Thread *p = (Thread *)arg;
    p->run();
    return NULL;
}
void Thread::run()
{
    //监听base事件集合
    //event_base_dispatch()内部封装了一个死循环来处理事件 类似与QT的exec()
    //如果m_base集合为空，则event_base_dispatch会立即返回
    //所以初始化时需要给m_base添加一个事件
    event_base_dispatch(m_base);
    event_base_free(m_base);
}
event_base * Thread::getBase()
{
    return m_base;
}
