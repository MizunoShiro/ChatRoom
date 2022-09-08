#include "user.h"

User::User(std::string n, std::string p, int r, TcpSocket *s)
    : _usrName(n), _passWd(p), _rank(r), _s(s)
{

}
TcpSocket *User::getSocket()
{
    return _s;
}
const char *User::getName()
{
    return _usrName.c_str();
}
int User::getRank()
{
    return _rank;
}
void User::setRank(int rank)
{
    if(rank <= 0)
    {
        _rank = 0;
    }
    _rank = rank;
}
