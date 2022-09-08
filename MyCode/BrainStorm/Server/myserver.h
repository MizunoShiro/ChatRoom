#ifndef MyServer_H
#define MyServer_H

#include "tcpserver.h"
#include "mydatabase.h"
#include "mysignal.h"
#include "user.h"
#include "tcptransfer.h"
#include <spdlog/spdlog.h>
#include <json/json.h>
#include <json/reader.h>
#include <string>
#include <map>

#define DEBUG
class MyServer:public TcpServer
{
public:
    MyServer();

    void connectEvent(TcpSocket *s);
    void readEvent(TcpSocket *s);
    void writeEvent(TcpSocket *);
    void closeEvent(TcpSocket *s,short what);
private:
    //用户注册
    void Register(TcpSocket *s,const Json::Value &inJson);

    //用户登录
    void Login(TcpSocket *s,const Json::Value &inJson);

    //管理员登录
    void rootLogin(TcpSocket *s,const Json::Value &inJson);

    //发送数据
    void writeData(TcpSocket *s,const Json::Value &inJson);

    //个人训练获取题目
    void GetQuestion(TcpSocket *s);

    //初始化rank分数对照表
    void initRankMap();

    //进行排位
    void Rank(TcpSocket *s);

    //开始对决
    void startRank(TcpSocket *first, TcpSocket *second);

    //接收排位玩家数据
    void rankAnswerOneQuestion(TcpSocket *s, const Json::Value &inJson);

    //接收排位结果
    void rankResult(TcpSocket *s, const Json::Value &inJosn);

    //发送在线用户
    void onlineRefresh(TcpSocket *s);

    //转发广告信息(通知客户端更新广告)
    void Transmit(TcpSocket *s, const Json::Value &inJson);

private:
    std::shared_ptr<spdlog::logger> _log;   //记录日志的句柄
    MyDataBase *_db;    //数据库句柄

    std::mutex _usrLock;
    //键是用户名，值是用户指针
    std::map<std::string, User *> _users;   //在线用户列表

    //键是rank分数，值是段位
    std::map<int, std::string> _rankMap;    //rank分数比对表

    std::mutex _rankLock;
    //键是rank分数，值是等待中用户socket句柄
    std::map<int, TcpSocket *> _rankQueue;  //匹配队列

    int _rootFlag;   //记录管理员状态
    
};

#endif // MyServer_H
