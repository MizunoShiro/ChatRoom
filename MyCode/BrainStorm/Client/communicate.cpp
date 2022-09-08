#include "communicate.h"

Communicate::Communicate(QObject *parent) : QObject(parent)
{
    //连接服务器
    s.connectToHost(QHostAddress("192.168.12.128"), 6666);
    connect(&s, SIGNAL(readyRead()), this, SLOT(readData()));
}

void Communicate::readData()
{
    QByteArray data;
    while (s.bytesAvailable())
    {
        data += s.readAll();
    }
    //数据解析
    QJsonDocument dt = QJsonDocument::fromJson(data);
    if(dt.isNull())
    {
        return;
    }
    QJsonObject root = dt.object();

    int cmd = root["cmd"].toInt();
    switch (cmd)
    {
     case REGISTER:
        emit rstResult(root["result"].toInt());
        break;
    case LOGIN:
        emit rstLogin(root);
        break;
    case ALONE_GETQUESTION:
        emit receiveAloneQuestion(root["question"].toObject());
        break;
    case RANK:
        emit Rank(root);
        break;
    case ANSWER:
        emit Rank(root);
        break;
    case RANKRESULT:
        emit Rank(root);
        break;
    case ROOTLOGIN:
        emit rstRootLogin(root);
        break;
    case REFRESH:
        emit onlineRefresh(root);
        break;
    case IMAGE:
        emit advertisement(root);
        break;
    case OFFLINE:
        emit enemyOffline(root);
        break;
     default:
        break;
    }

}
void Communicate::writeData(const QJsonObject &Json)
{
    QJsonDocument d(Json);
    QByteArray sendData = d.toJson();
    int len = sendData.size();

    s.write((char *)&len, sizeof(int));  //发送数据长度
    s.write(sendData);                  //发送数据
}
