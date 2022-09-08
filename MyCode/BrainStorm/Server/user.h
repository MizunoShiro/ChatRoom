#ifndef USER_H
#define USER_H

#include <string>
#include "tcpsocket.h"

class User
{
public:
    User(std::string name, std::string passwd, int r, TcpSocket *s);

    TcpSocket *getSocket();
    const char *getName();
    int getRank();
    void setRank(int rank);

private:
    std::string _usrName;
    std::string _passWd;
    int _rank;

    TcpSocket *_s;
};

#endif // USER_H
