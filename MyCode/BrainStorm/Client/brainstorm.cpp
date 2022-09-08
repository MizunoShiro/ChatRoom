#include "brainstorm.h"
#include "ui_brainstorm.h"

BrainStorm::BrainStorm(Communicate *com, QJsonObject &json, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrainStorm)
{
    ui->setupUi(this);
    this->setWindowTitle("BrainStorm");
    this->setWindowIcon(QIcon("D:/MinGW/2022C++/BrainStorm/advertisement/brainstorm.webp"));
    ui->stackedWidget->setCurrentWidget(ui->Homepage);
    advertisementInit();

    _com = com;

    _myName = json["username"].toString();
    _myRank = json["rank"].toString();

    connect(&_aloneTimer, SIGNAL(timeout()), this, SLOT(aloneTimeOut()));
    connect(_com, SIGNAL(receiveAloneQuestion(QJsonObject )), this, SLOT(receiveAloneQuestion(QJsonObject )));
    connect(_com, SIGNAL(Rank(QJsonObject &)), this, SLOT(Rank(QJsonObject &)));
    connect(&_rankTimer, SIGNAL(timeout()), this, SLOT(rankTimeOut()));
    connect(_com, &Communicate::enemyOffline, this, &BrainStorm::RstRank);
    connect(_com, &Communicate::advertisement, [this](QJsonObject &Json){
        size = Json["size"].toInt();
        qDebug()<<size;
        m_s.connectToHost(QHostAddress("192.168.12.128"), 6667);
        file = new QFile("../advertisement/1.jpg");
        file->open(QIODevice::WriteOnly);
        int flag = 0;
        m_s.write((char *)&flag, sizeof(flag));

    });
    connect(&m_s, &QTcpSocket::readyRead, [this](){

        Image.append(m_s.readAll());

        if (Image.size() >= size)
        {
            file->write(Image);
            file->close();
            m_s.abort();
            Image.clear();
            advertisementInit();
        }
    });
}

BrainStorm::~BrainStorm()
{
    delete _img;
    delete ui;
}
//开始答题
void BrainStorm::on_StartButton_clicked()
{
    this->GetQuestion();
}
//返回主页
void BrainStorm::on_ReturnButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->Homepage);
}

void BrainStorm::advertisementInit()
{
   QPixmap pix("../advertisement/1.jpg");
   ui->ImageLabel_1->setPixmap(pix);
   ui->ImageLabel_2->setPixmap(pix);
   ui->ImageLabel_3->setPixmap(pix);
   ui->ImageLabel_1->setScaledContents(true);
   ui->ImageLabel_2->setScaledContents(true);
   ui->ImageLabel_3->setScaledContents(true);
}



/*-----------------------------个人训练----------------------------*/
//开启训练
void BrainStorm::on_TrainButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->Userpage);
}
//获取问题
void BrainStorm::GetQuestion()
{
    QJsonObject Json;
    Json["cmd"] = ALONE_GETQUESTION;

    _com->writeData(Json);
}
//接收题目
void BrainStorm::receiveAloneQuestion(QJsonObject Json)
{
    _aloneQuestion = Json;
    _currentAloneQuestion = 0;
    _aloneScore = 0;
    _aloneSec = 15;
    ui->lcdNumber->display(_aloneSec);
    aloneSetQuestion();
    //开启定时器
    _aloneTimer.start(1000);
    ui->stackedWidget->setCurrentWidget(ui->Answerpage);
}
//设置问题
void BrainStorm::aloneSetQuestion()
{
    if(_currentAloneQuestion == QUESTION_NUM)
    {
        ui->Resultlabel->setText("恭喜全部答对");
        QString str = QString("本次得分: %1").arg(_aloneScore);
        ui->Scorelabel->setText(str);
        _aloneTimer.stop();
        ui->stackedWidget->setCurrentWidget(ui->Scorepage);
    }
    ui->Questionlabel->setText(_aloneQuestion["question"].toArray().at(_currentAloneQuestion).toString());
    ui->SelectAButton->setText(_aloneQuestion["selectionA"].toArray().at(_currentAloneQuestion).toString());
    ui->SelectBButton->setText(_aloneQuestion["selectionB"].toArray().at(_currentAloneQuestion).toString());
    ui->SelectCButton->setText(_aloneQuestion["selectionC"].toArray().at(_currentAloneQuestion).toString());
    ui->SelectDButton->setText(_aloneQuestion["selectionD"].toArray().at(_currentAloneQuestion).toString());
}
//回答问题
void BrainStorm::aloneAnswerQuestion(int select)
{
    if(select == _aloneQuestion["answer"].toArray().at(_currentAloneQuestion).toString().toInt())
    {
        _aloneScore += 20;
        _currentAloneQuestion++;
        aloneSetQuestion();
        _aloneSec = 15;
        _aloneTimer.stop();
        ui->lcdNumber->display(_aloneSec);
        _aloneTimer.start(1000);
    }
    else
    {
        ui->Resultlabel->setText("回答错误");
        QString str = QString("本次得分: %1").arg(_aloneScore);
        ui->Scorelabel->setText(str);
        _aloneTimer.stop();
        ui->stackedWidget->setCurrentWidget(ui->Scorepage);
    }
}
//计时器
void BrainStorm::aloneTimeOut()
{
    --_aloneSec;
    if(_aloneSec == 0)
    {
        ui->Resultlabel->setText("回答错误");
        QString str = QString("本次得分: %1").arg(_aloneScore);
        ui->Scorelabel->setText(str);
        _aloneTimer.stop();
        ui->stackedWidget->setCurrentWidget(ui->Scorepage);
    }
    ui->lcdNumber->display(_aloneSec);
}
//选项
void BrainStorm::on_SelectAButton_clicked()
{
    aloneAnswerQuestion(1);
}
void BrainStorm::on_SelectBButton_clicked()
{
    aloneAnswerQuestion(2);
}
void BrainStorm::on_SelectCButton_clicked()
{
    aloneAnswerQuestion(3);
}
void BrainStorm::on_SelectDButton_clicked()
{
    aloneAnswerQuestion(4);
}
//答题结束，返回首页
void BrainStorm::on_ReturnAnswerButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->Homepage);
}

/*-----------------------------排位赛----------------------------*/
//开启排位
void BrainStorm::on_RankButton_clicked()
{
    QJsonObject Json;
    Json["cmd"] = RANK;
    _com->writeData(Json);

    ui->stackedWidget->setCurrentWidget(ui->Rank_Wait);
}

//排位赛
void BrainStorm::Rank(QJsonObject &Json)
{
    int cmd = Json["cmd"].toInt();
    if(cmd == RANK)
    {
        _rankQuestion = Json["question"].toObject();
        _enemyName = Json["enemyName"].toString();
        _enemyRank = Json["enemyRank"].toString();
        _enemyScore = Json["enemyScore"].toInt();

        _rankSec = 10;
        _myScore = 0;
        _currentRankQuestion = 0;
        _enemycurrentQuestion = 0;
        ui->lcdNumber_Rank->display(_rankSec);
        setEnemyScore();
        setMyScore();
        rankSetQuestion();

        _rankTimer.start(1000);
        ui->stackedWidget->setCurrentWidget(ui->Rank_Answer);
    }
    else if(cmd == ANSWER)
    {
        _enemyScore           = Json["enemyScore"].toInt();
        _enemycurrentQuestion = Json["enemyQuestionId"].toInt();
        _enemyleftSec         = Json["enemyleftSec"].toInt();

        setEnemyScore();

        if(_currentRankQuestion == _enemycurrentQuestion)
        {
            _rankTimer.stop();
            _rankSec = 15;
            ui->lcdNumber_Rank->display(_rankSec);
            _rankTimer.start(1000);
            rankSetQuestion();
        }
    }
    else if(cmd == RANKRESULT)
    {
        rankSetResult(Json);
    }
}

//设置排位赛分数显示
void BrainStorm::setEnemyScore()
{
    QString str = QString("%1(%2):%3").arg(_enemyName, -5).arg(_enemyRank).arg(_enemyScore);
    ui->EnemyRanklabel->setText(str);
}

void BrainStorm::setMyScore()
{
    QString str = QString("%1(%2):%3").arg(_myName, -5).arg(_myRank).arg(_myScore);
    ui->MyRanklabel->setText(str);
}

//设置问题
void BrainStorm::rankSetQuestion()
{
    if(_currentRankQuestion == QUESTION_NUM)
    {
        _rankTimer.stop();
        QJsonObject Json;
        Json["cmd"]             = RANKRESULT;
        Json["Score"]           = _myScore;
        Json["enemyScore"]      = _enemyScore;
        Json["enemyName"]       = _enemyName;
        Json["myleftSec"]       = _myleftSec;
        Json["enemyleftSec"]    = _enemyleftSec;
        _com->writeData(Json);

    }
    ui->Questionlabel_Rank->setText(_rankQuestion["question"].toArray().at(_currentRankQuestion).toString());
    ui->SelectAButton_Rank->setText(_rankQuestion["selectionA"].toArray().at(_currentRankQuestion).toString());
    ui->SelectBButton_Rank->setText(_rankQuestion["selectionB"].toArray().at(_currentRankQuestion).toString());
    ui->SelectCButton_Rank->setText(_rankQuestion["selectionC"].toArray().at(_currentRankQuestion).toString());
    ui->SelectDButton_Rank->setText(_rankQuestion["selectionD"].toArray().at(_currentRankQuestion).toString());

    ui->SelectAButton_Rank->setEnabled(true);
    ui->SelectBButton_Rank->setEnabled(true);
    ui->SelectCButton_Rank->setEnabled(true);
    ui->SelectDButton_Rank->setEnabled(true);

    ui->SelectAButton_Rank->setStyleSheet("");
    ui->SelectBButton_Rank->setStyleSheet("");
    ui->SelectCButton_Rank->setStyleSheet("");
    ui->SelectDButton_Rank->setStyleSheet("");
}

//计时器
void BrainStorm::rankTimeOut()
{
    --_rankSec;
    if(_rankSec == 0)
    {
        if(ui->SelectAButton_Rank->isEnabled())
        {
            _currentRankQuestion++;
        }
        rankSetQuestion();
        _rankSec = 15;
    }
    ui->lcdNumber_Rank->display(_rankSec);
}

//回答问题
void BrainStorm::rankAnswerQuestion(int select)
{
    if(select == _rankQuestion["answer"].toArray().at(_currentRankQuestion).toString().toInt())
    {
        _myScore += 20;
        _currentRankQuestion++;
        _myleftSec += _rankSec;
        setMyScore();
    }
    else
    {
        _currentRankQuestion++;
        _myleftSec += _rankSec;
    }
    QJsonObject Json;
    Json["cmd"]             = ANSWER;
    Json["enemyName"]       = _enemyName;
    Json["enemyQuestionId"] = _currentRankQuestion;
    Json["enemyScore"]      = _myScore;
    Json["enemyleftSec"]    = _myleftSec;
    _com->writeData(Json);
    //判断是否跳转到下一题
    if(_currentRankQuestion == _enemycurrentQuestion)
    {
        rankSetQuestion();
        _rankTimer.stop();
        _rankSec = 15;
        ui->lcdNumber_Rank->display(_rankSec);
        _rankTimer.start(1000);
    }

}

void BrainStorm::on_SelectAButton_Rank_clicked()
{
    ui->SelectAButton_Rank->setStyleSheet("background-color: rgb(52, 101, 164)");
    ui->SelectAButton_Rank->setEnabled(false);
    ui->SelectBButton_Rank->setEnabled(false);
    ui->SelectCButton_Rank->setEnabled(false);
    ui->SelectDButton_Rank->setEnabled(false);
    rankAnswerQuestion(1);
}
void BrainStorm::on_SelectBButton_Rank_clicked()
{
    ui->SelectBButton_Rank->setStyleSheet("background-color: rgb(52, 101, 164)");
    ui->SelectAButton_Rank->setEnabled(false);
    ui->SelectBButton_Rank->setEnabled(false);
    ui->SelectCButton_Rank->setEnabled(false);
    ui->SelectDButton_Rank->setEnabled(false);
    rankAnswerQuestion(2);
}
void BrainStorm::on_SelectCButton_Rank_clicked()
{
    ui->SelectCButton_Rank->setStyleSheet("background-color: rgb(52, 101, 164)");
    ui->SelectAButton_Rank->setEnabled(false);
    ui->SelectBButton_Rank->setEnabled(false);
    ui->SelectCButton_Rank->setEnabled(false);
    ui->SelectDButton_Rank->setEnabled(false);
    rankAnswerQuestion(3);
}
void BrainStorm::on_SelectDButton_Rank_clicked()
{
    ui->SelectDButton_Rank->setStyleSheet("background-color: rgb(52, 101, 164)");
    ui->SelectAButton_Rank->setEnabled(false);
    ui->SelectBButton_Rank->setEnabled(false);
    ui->SelectCButton_Rank->setEnabled(false);
    ui->SelectDButton_Rank->setEnabled(false);
    rankAnswerQuestion(4);
}

void BrainStorm::rankSetResult(QJsonObject &json)
{
    QString newRank = json["newRank"].toString();
    if(_myScore == _enemyScore && _myleftSec == _enemyleftSec)
    {
        ui->Resultlabel_Rank->setText("平局");
    }
    else if(_myScore > _enemyScore || (_myScore == _enemyScore && _myleftSec > _enemyleftSec))
    {
        ui->Resultlabel_Rank->setText("胜利");
    }
    if(_myScore < _enemyScore || (_myScore == _enemyScore && _myleftSec < _enemyleftSec))
    {
        ui->Resultlabel_Rank->setText("失败");
    }

    QString str = QString("%1 ----> %2").arg(_myRank).arg(newRank);
    ui->Scorelabel_Rank->setText(str);
    _myRank = newRank;

    ui->stackedWidget->setCurrentWidget(ui->Rank_Result);
}

void BrainStorm::on_ReturnAnswerButton_Rank_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->Homepage);
}

void BrainStorm::RstRank(QJsonObject &Json)
{
    qDebug()<<"11111111";
    _rankTimer.stop();
    _rankSec = 0;
    QString newRank = Json["newRank"].toString();
    ui->Resultlabel_Rank->setText("胜利");
    QString str = QString("%1 ----> %2").arg(_myRank).arg(newRank);
    ui->Scorelabel_Rank->setText(str);
    _myRank = newRank;
    ui->stackedWidget->setCurrentWidget(ui->Rank_Result);

}
