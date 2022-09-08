#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostAddress>
#include "mysignal.h"

class Communicate : public QObject
{
    Q_OBJECT
public:
    explicit Communicate(QObject *parent = nullptr);

    void writeData(const QJsonObject &Json);

signals:
    void rstResult(int );   //发送注册结果
    void rstLogin(QJsonObject &);   //发送登录结果
    void rstRootRank(QJsonObject &);    //发送管理员登录结果
    void receiveAloneQuestion(QJsonObject);   //发送获取题目结果
    void Rank(QJsonObject &);    //发送排位赛请求结果
    void rstRootLogin(QJsonObject &);   //发送管理员登录结果
    void onlineRefresh(QJsonObject &);  //发送在线用户查询结果
    void advertisement(QJsonObject &);  //发送用户更新广告
    void enemyOffline(QJsonObject &);   //对手掉线信号
public slots:
    void readData();
private:
    QTcpSocket s;
};

#endif // COMMUNICATE_H
