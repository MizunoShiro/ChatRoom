#include "tcptransfer.h"

pthread_mutex_t TcpTransfer::m_mutex = PTHREAD_MUTEX_INITIALIZER;
TcpTransfer::TcpTransfer()
{
//#ifdef DEBUG
//    _log = spdlog::get("BrainStorm");
//#else
//    _log = spdlog::rotating_logger_mt("BrainStorm", "./../MyCode/Server/BrainStorm", 1024*1024 * 5, 3);
//    _log->flush_on(spdlog::level::info);
//#endif
}

int TcpTransfer::serverCreate()
{

    //signal(SIGPIPE, SIG_IGN);
    int Transfd = socket(AF_INET, SOCK_STREAM, 0);
    if(Transfd == -1)
    {
        return -1;
    }
    int opt = 1;
    setsockopt(Transfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
    sockaddr_in trans_addr;
    trans_addr.sin_family = AF_INET;
    trans_addr.sin_port = htons(PORT);
    trans_addr.sin_addr.s_addr = inet_addr("192.168.12.128");
    int ret = bind(Transfd, (sockaddr *)&trans_addr, sizeof(trans_addr));
    if (ret == -1)
    {
        return -1;
    }
    if(listen(Transfd, 21) == -1)
    {
        return -1;
    }
    return Transfd;
}

void TcpTransfer::initNet()
{
    int Transfd = serverCreate();

    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    while (1)
    { 
        int fd = accept(Transfd, (struct sockaddr *)&client_addr, &len);
        if(fd == -1)
        {
            return;
        }

        pthread_t pid;
        if(pthread_create(&pid, NULL, ThreadStart, (void *)&fd) != 0)
        {
            return;
        }
        pthread_detach(pid);
    }
}

void* TcpTransfer::ThreadStart(void *fd)
{
    int _fd = *(int *)fd;
    int flag = -1;
    recv(_fd, &flag, sizeof(flag), 0);
    int ret = 0;
    int tatol = 0;
    char buf[4096] = {0};
    if(flag == 1)
    {
        std::ofstream ofs;
        ofs.open("./1.jpg", std::ios_base::out | std::ios_base::binary);
        int size = 0;
        recv(_fd, &size, sizeof(size), 0);
        while(tatol < size)
        {
            ret = recv(_fd, buf, sizeof(buf), 0);
            if(ret <= 0)
            {
                break;
            }
            ofs.write(buf, ret);
            tatol += ret;
            memset(buf, 0, sizeof(buf));
        }
        ofs.close();
        printf("文件传输成功\n");
        flag = 1;
        send(_fd, &flag, sizeof(flag), 0);
    }
    else if(flag == 0)
    {
        printf("user\n");
        std::ifstream ifs;
        ifs.open("./1.jpg", std::ios_base::in | std::ios_base::binary);
        struct stat statbuf;
        stat("./1.jpg", &statbuf);
        int size = statbuf.st_size;
        printf("size = %d\n",size);
        char Image[size] = {0};
        ifs.read(Image, size);
        ifs.close();
        printf("%ld\n",sizeof(Image));
        send(_fd, Image, size, 0);

    }
    return NULL;
}
