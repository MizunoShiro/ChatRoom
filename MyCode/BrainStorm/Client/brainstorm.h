#ifndef BRAINSTORM_H
#define BRAINSTORM_H

#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTimer>
#include "communicate.h"
#include "imagetransfer.h"
namespace Ui {
class BrainStorm;
}

class BrainStorm : public QDialog
{
    Q_OBJECT

public:
    explicit BrainStorm(Communicate *com, QJsonObject &json, QWidget *parent = 0);
    ~BrainStorm();

private slots:
    void on_StartButton_clicked();

    void on_ReturnButton_clicked();

    void on_TrainButton_clicked();

    /*---单人训练---*/
    //从服务器接收题目
    void receiveAloneQuestion(QJsonObject Json);

    //计时器
    void aloneTimeOut();

    void on_ReturnAnswerButton_clicked();

    void on_SelectAButton_clicked();
    void on_SelectBButton_clicked();
    void on_SelectCButton_clicked();
    void on_SelectDButton_clicked();

    /*---排位---*/
    void on_RankButton_clicked();

    //接收排位赛
    void Rank(QJsonObject &Json);

    //设置排位赛分数显示
    void setEnemyScore();
    void setMyScore();

    //计时器
    void rankTimeOut();

    //处理对手掉线
    void RstRank(QJsonObject &Json);


    void on_SelectAButton_Rank_clicked();

    void on_SelectBButton_Rank_clicked();

    void on_SelectCButton_Rank_clicked();

    void on_SelectDButton_Rank_clicked();

    void on_ReturnAnswerButton_Rank_clicked();

private:

    //从服务器获取题目
    void GetQuestion();

    /*---单人训练---*/
    //设置题目
    void aloneSetQuestion();

    //回答题目
    void aloneAnswerQuestion(int select);

    /*---排位赛---*/
    //设置问题
    void rankSetQuestion();

    //回答问题
    void rankAnswerQuestion(int select);

    //处理结果
    void rankSetResult(QJsonObject &json);
    
    //初始化广告
    void advertisementInit();

private:
    Ui::BrainStorm *ui;
    Communicate *_com;
    
    ImageTransfer *_img;
    QFile *file;

    /*---个人训练---*/
    QJsonObject _aloneQuestion;  //问题
    int _currentAloneQuestion;   //问题下标

    QTimer _aloneTimer;         //训练定时器
    int _aloneSec;              //个人训练答题时间
    int _aloneScore;            //个人训练得分

    /*---排位赛---*/
    QJsonObject _rankQuestion;  //问题
    int _currentRankQuestion;   //问题下标
    int _enemycurrentQuestion;  //对手问题下标

    QString _enemyName; //对手名字
    QString _enemyRank; //对手段位
    int _enemyScore;    //对手分数

    QString _myName;
    QString _myRank;

    QTimer _rankTimer;         //定时器
    int _myleftSec;              //剩余时间
    int _enemyleftSec;         //对手剩余时间
    int _rankSec;              //答题时间
    int _myScore;            //个人得分

    QTcpSocket m_s;
    int size;
    QByteArray Image;

};

#endif // BRAINSTORM_H
