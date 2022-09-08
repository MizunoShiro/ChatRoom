#include "myserver.h"


MyServer::MyServer()
{
    initRankMap();
    _rootFlag = 0;
#ifdef DEBUG
    _log = spdlog::stdout_color_mt("BrainStorm");
#else
    _log = spdlog::rotating_logger_mt("BrainStorm", "./../MyCode/Server/BrainStorm", 1024*1024 * 5, 3);
    _log->flush_on(spdlog::level::info);
#endif

    _db = new MyDataBase(NULL, "brainstorm", "123456", "brainstorm");

}

void MyServer::connectEvent(TcpSocket *s)
{
    _log->info("客户端连接[{}:{}]", s->getIp(), s->getPort());
}

void MyServer::readEvent(TcpSocket *s)
{
    char buf[BUF_SIZE];
    while (1)
    {
        int len = 0;
        s->readData(&len, sizeof(len));
        if(len <= 0)
        {
            break;
        }
        s->readData(buf,len);
        Json::Value root;
        Json::Reader reader;    //json解析器
        if(!reader.parse(buf,root))
        {
            _log->error("json数据解析失败");
            return;
        }
        int cmd = root["cmd"].asInt();
        switch (cmd)
        {
        case REGISTER:
            Register(s,root);
            break;
        case LOGIN:
            Login(s,root);
            break;
        case ALONE_GETQUESTION:
            GetQuestion(s);
            break;
        case RANK:
            Rank(s);
            break;
        case ANSWER:
            rankAnswerOneQuestion(s, root);
            break;
        case RANKRESULT:
            rankResult(s, root);
            break;
        case ROOTLOGIN:
            rootLogin(s, root);
            break;
        case REFRESH:
            onlineRefresh(s);
            break;
        case IMAGE:
            Transmit(s, root);
            break;
        default:
            break;
        }

    }
}

void MyServer::Transmit(TcpSocket *, const Json::Value &inJson)
{
    for(auto it = _users.begin(); it != _users.end(); it++)
    {
        writeData(it->second->getSocket(), inJson);
    }
    _log->info("通知用户更新广告成功");
}

void MyServer::writeEvent(TcpSocket *)
{

}

void MyServer::closeEvent(TcpSocket *s,short)
{
    if(!strcmp(s->getName().c_str(), "root"))
    {
        _rootFlag = 0;
        _log->info("管理员[{}:{}]退出", s->getIp(), s->getPort());

    }
    else if(s->getName().empty())
    {
        _log->info("客户端[{}:{}]退出", s->getIp(), s->getPort());
    }
    else
    {
        //将用户从等待 rank的队列中删除
        {
            std::unique_lock<std::mutex> lock(_rankLock);
            int rank = _users[s->getName()]->getRank();
            auto it = _rankQueue.find(rank);
            if(it != _rankQueue.end())
            {
                _rankQueue.erase(it);
            }
        }
        std::unique_lock<std::mutex> lock(_usrLock);
        auto it = _users.begin();
        while (it != _users.end())
        {
            if(it->second->getSocket() == s)
            {
                _users.erase(it);
                _log->info("用户{}[{}:{}]退出", it->second->getName(), s->getIp(), s->getPort());

                delete it->second;
                return;
            }
            it++;
        }
    }

}

void MyServer::Register(TcpSocket *s,const Json::Value &inJson)
{
    std::string usrName = inJson["username"].asString();
    std::string passWd = inJson["password"].asString();

    //检测用户是否已经存在
    char sql[SQL_SIZE] = {0};
    sprintf(sql, "select *from user where name = '%s' and password = '%s'", usrName.c_str(), passWd.c_str());
    int result = OK;
    Json::Value outJson;
    bool ret = _db->db_select(sql, outJson);
    if(!ret)
    {
        result = ERROR;
        _log->error("Register select user error!");
    }

    if(outJson.isMember("name"))  //用户存在，表明已经注册过了
    {
        result = ERROR_EXIST;
    }
    else
    {
        //将注册的用户信息保存到数据库
        sprintf(sql, "insert into user(name, password, rank) values('%s', '%s', 0)", usrName.c_str(), passWd.c_str());
        ret = _db->db_exec(sql);
        if(!ret)
        {
            result = ERROR;
            _log->error("Register insert user error!");
        }
        else
        {
            _log->info("Register user = {} success!", usrName);
        }
    }
    Json::Value Json;
    Json["cmd"]    = REGISTER;
    Json["result"] = result;
    writeData(s, Json);
}

void MyServer::Login(TcpSocket *s,const Json::Value &inJson)
{
    std::string usrName = inJson["username"].asString();
    std::string passwd = inJson["password"].asString();

    char sql[SQL_SIZE];
    sprintf(sql, "select *from user where name = '%s' and password = '%s'", usrName.c_str(), passwd.c_str());
    int result = OK;
    int rank = 0;
    Json::Value outJson;
    bool ret = _db->db_select(sql,outJson);
    if(!ret)
    {
        result = ERROR;
        _log->error("Login select user error!");
    }
    if(outJson.isMember("name"))    //用户存在，可以登录
    {
        std::unique_lock<std::mutex> lock(_usrLock);
        if(_users.find(usrName) != _users.end())    //用户已登录
        {
            result = ERROR_LOGIN;
        }
        else
        {
            int i = 0;
            rank = atoi(outJson["rank"][i].asString().c_str());
            User *user = new User(usrName, passwd, rank, s);
            _users.insert(make_pair(usrName, user));
            _log->info("用户{}[{}:{}] Login", usrName, s->getIp(), s->getPort());
            s->setUserName(usrName);
        }
    }
    else
    {
        result = NAMEORPASSWD;
    }

    Json::Value Json;
    Json["cmd"]      = LOGIN;
    Json["result"]   = result;
    Json["username"] = usrName;
    Json["rank"]     = _rankMap[rank];
    writeData(s, Json);
}

void MyServer::rootLogin(TcpSocket *s,const Json::Value &inJson)
{
    std::string rootName = inJson["username"].asString();
    std::string passwd = inJson["password"].asString();
    int result = OK;
    Json::Value Json;
    if(rootName.compare("root") == 0 && passwd.compare("123") == 0)
    {
        if(_rootFlag != 0)
        {
            result = ERROR_LOGIN;
        }
        else
        {
            _rootFlag = 1;
            s->setUserName("root");
            _log->info("管理员{}[{}:{}] Login", rootName, s->getIp(), s->getPort());

            for(auto it = _users.begin(); it != _users.end(); it++)
            {
                Json["name"].append(it->first);
                Json["rank"].append(_rankMap[it->second->getRank()]);
            }
        }
    }
    else
    {
        result = NAMEORPASSWD;
    }
    Json["cmd"]      = ROOTLOGIN;
    Json["result"]   = result;
    writeData(s, Json);
}

void MyServer::writeData(TcpSocket *s,const Json::Value &inJson)
{
    std::string data = inJson.toStyledString();
    s->writeData(data.c_str(), data.length());
}

void MyServer::GetQuestion(TcpSocket *s)
{
    char sql[SQL_SIZE] = {0};
    sprintf(sql, "select *from question order by rand() limit %d",QUESTION_NUM);

    int result = OK;
    Json::Value outJson;
    bool ret = _db->db_select(sql, outJson);
    if(!ret || outJson["question"].size() != QUESTION_NUM)
    {
        result = ERROR;
        _log->error("Question select error!");
    }

    Json::Value Json;
    Json["cmd"]      = ALONE_GETQUESTION;
    Json["result"]   = result;
    Json["question"] = outJson;

    _log->info("用户{}[{}:{}]获取题目:{}\n",s->getName(), s->getIp(), s->getPort(), Json.toStyledString());
    writeData(s, Json);
}
//rank分数对照表
void MyServer::initRankMap()
{
    char buf[BUF_SIZE] = {0};
    int rank = 0;
    int num = 0;
    for(int i = 0; i < 100; i++)
    {
        if(i < 9)
        {
            rank = i / 3;
            num = i % 3;
            sprintf(buf,"青铜%d %d颗星", 3 - rank,  num + 1);
            _rankMap.insert(std::make_pair(i, buf));
        }
        else if(9 <= i && i < 18)
        {
            rank = (i - 9) / 3;
            num = (i - 9) % 3;
            sprintf(buf,"白银%d %d颗星", 3 - rank,  num + 1);
            _rankMap.insert(std::make_pair(i, buf));
        }
        else if(18 <= i && i < 34)
        {
            rank = (i - 18) / 4;
            num = (i - 18) % 4;
            sprintf(buf,"黄金%d %d颗星", 4 - rank,  num + 1);
            _rankMap.insert(std::make_pair(i, buf));
        }
        else if(34 <= i && i < 50)
        {
            rank = (i - 34) / 4;
            num = (i - 34) % 4;
            sprintf(buf,"铂金%d %d颗星", 4 - rank,  num + 1);
            _rankMap.insert(std::make_pair(i, buf));
        }
        else if(50 <= i && i < 75)
        {
            rank = (i - 50) / 5;
            num = (i - 50) % 5;
            sprintf(buf,"钻石%d %d颗星", 5 - rank,  num + 1);
            _rankMap.insert(std::make_pair(i, buf));
        }
        else if(75 <= i && i < 100)
        {
            rank = (i - 75) / 5;
            num = (i - 75) % 5;
            sprintf(buf,"星耀%d %d颗星", 5 - rank,  num + 1);
            _rankMap.insert(std::make_pair(i, buf));
        }
    }
}
//进行排位
void MyServer::Rank(TcpSocket *s)
{
    TcpSocket *other = NULL; //对手socket
    int rank = _users[s->getName()]->getRank();     //当前用户rank分数

    //查找对手
    std::unique_lock<std::mutex> lock(_rankLock);
    //查找同一段位对手
    auto it = _rankQueue.find(rank);
    if(it != _rankQueue.end())
    {
        other = it->second;
        _rankQueue.erase(it);
    }
    else
    {
        //查找rank分数相差五分以内的对手
        for(int i = 0; i <= 5; i++)
        {
            auto it = _rankQueue.find(rank + i);
            if(it != _rankQueue.end())
            {
                other = it->second;
                _rankQueue.erase(it);
                break;
            }
            it = _rankQueue.find(rank - i);
            if(it != _rankQueue.end())
            {
                other = it->second;
                _rankQueue.erase(it);
                break;
            }
        }
    }
    if(other == NULL)   //未匹配到用户
    {
        _rankQueue.insert(std::make_pair(rank, s));
        _log->info("当前等候rank人数: {}", _rankQueue.size());
    }
    else    //开始对决
    {
        startRank(s, other);
    }
}
//开始对决
void MyServer::startRank(TcpSocket *first, TcpSocket *second)
{
    char sql[SQL_SIZE] = {0};
    sprintf(sql, "select *from question order by rand() limit %d",QUESTION_NUM);

    int result = OK;
    Json::Value outJson;
    bool ret = _db->db_select(sql, outJson);
    if(!ret || outJson["question"].size() != QUESTION_NUM)
    {
        result = ERROR;
        _log->error("Question select error!");
    }

    Json::Value Json;
    Json["cmd"]      = RANK;
    Json["result"]   = result;
    Json["question"] = outJson;

    //first user
    Json["enemyName"]  = second->getName();
    Json["enemyRank"]  = _rankMap[_users[second->getName()]->getRank()];
    Json["enemyScore"] = 0;
    writeData(first, Json);

    //second user
    Json["enemyName"]  = first->getName();
    Json["enemyRank"]  = _rankMap[_users[first->getName()]->getRank()];
    Json["enemyScore"] = 0;
    writeData(second, Json);

    _log->info("获取rank题目:{}\n", Json.toStyledString());
}
//接收排位玩家数据
void MyServer::rankAnswerOneQuestion(TcpSocket *s, const Json::Value &inJson)
{
    std::string enemyName = inJson["enemyName"].asString();
    if(_users.find(enemyName) == _users.end())
    //if(_users[enemyName] == NULL)
    {
        _log->info("用户{}掉线",enemyName);
        int newRank = _users[s->getName()]->getRank() + 1;
        Json::Value Json;
        Json["cmd"]     = OFFLINE;
        Json["newRank"] = _rankMap[newRank];
        writeData(s, Json);
        //将段位变化保存到数据库中
        char sql[SQL_SIZE] = {0};
        sprintf(sql, "update user set rank = '%d' where name = '%s'", newRank, s->getName().c_str());
        bool ret = _db->db_exec(sql);
        if(!ret)
        {
            _log->error("{} rankResult update error!", s->getName());
        }
        memset(sql, 0, sizeof(sql));
        sprintf(sql, "update user set rank = IF(rank < 1, 0, rank - 1) where name = '%s'", enemyName.c_str());
        ret = _db->db_exec(sql);
        if(!ret)
        {
            _log->error("{} rankResult update error!", s->getName());
        }
    }
    else
    {
        User *user = _users[enemyName];

        Json::Value Json;
        Json["cmd"] = ANSWER;
        Json["enemyScore"] = inJson["enemyScore"].asInt();
        Json["enemyQuestionId"] = inJson["enemyQuestionId"].asInt();
        Json["enemyleftSec"] = inJson["enemyleftSec"].asInt();

        writeData(user->getSocket(), Json);
    }
}
//接收排位结果
void MyServer::rankResult(TcpSocket *s, const Json::Value &inJson)
{
    std::string enemyName = inJson["enemyName"].asString();

    std::unique_lock<std::mutex> lock(_usrLock);
    User *user = _users[s->getName()];

    int myScore = inJson["Score"].asInt();
    int enemyScore = inJson["enemyScore"].asInt();

    int myleftSec = inJson["myleftSec"].asInt();
    int enemyleftSec = inJson["enemyleftSec"].asInt();
    int newRank;
    if(myScore > enemyScore)
    {
        newRank = user->getRank() + 1;
        user->setRank(newRank);
    }
    else if(myScore < enemyScore)
    {
        if(user->getRank() == 0)
        {
            newRank = 0;
        }
        else
        {
            newRank = user->getRank() - 1;
        }
        user->setRank(newRank);
    }
    else if(myScore == enemyScore)
    {
        if(enemyleftSec > myleftSec)
        {
            if(user->getRank() == 0)
            {
                newRank = 0;
            }
            else
            {
                newRank = user->getRank() - 1;
            }
            user->setRank(newRank);
        }
        else if(enemyleftSec < myleftSec)
        {
            newRank = user->getRank() + 1;
            user->setRank(newRank);
        }
    }
    Json::Value Json;
    Json["cmd"]     = RANKRESULT;
    Json["newRank"] = _rankMap[user->getRank()];

    writeData(user->getSocket(), Json);

    //将段位变化保存到数据库中
    char sql[SQL_SIZE] = {0};
    sprintf(sql, "update user set rank = '%d' where name = '%s'", newRank, s->getName().c_str());
    bool ret = _db->db_exec(sql);
    if(!ret)
    {
        _log->error("{} rankResult update error!", s->getName());
    }

}

//发送在线用户
void MyServer::onlineRefresh(TcpSocket *s)
{
    Json::Value Json;
    for(auto it = _users.begin(); it != _users.end(); it++)
    {
        Json["name"].append(it->first);
        Json["rank"].append(_rankMap[it->second->getRank()]);
    }
    Json["cmd"] = REFRESH;
    writeData(s, Json);
}


