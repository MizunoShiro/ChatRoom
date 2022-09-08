#include <iostream>
#include "thread.h"
#include "myserver.h"
#include "tcpserver.h"

using namespace std;

void *worker(void *arg)
{
    TcpTransfer *tcp = new TcpTransfer;
    tcp->initNet();
}

int main()
{
    pthread_t tid;
    if(0 != pthread_create(&tid, nullptr, worker, nullptr))
    {
        return -1;
    }

    MyServer s;
    s.listen(6666);
    s.start();
}
