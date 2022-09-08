#include "register.h"
#include "ui_register.h"

Register::Register(Communicate *com,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    this->setWindowTitle("注册");
    _com = com;
    this->setWindowIcon(QIcon("D:/MinGW/2022C++/BrainStorm/advertisement/brainstorm.webp"));
    connect(_com, SIGNAL(rstResult(int)), this, SLOT(rstResult(int)));
}

Register::~Register()
{
    delete ui;
}
void Register::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        on_RegisterButton_clicked();
    }
}

void Register::on_RegisterButton_clicked()
{
    QString userName = ui->NamelineEdit->text();
    QString passwd = ui->PasswordlineEdit->text();

    //给服务器发送注册信息
    QJsonObject json;
    json["cmd"]      = REGISTER;
    json["username"] = userName;
    json["password"] = passwd;

    _com->writeData(json);
}
//处理注册结果
void Register::rstResult(int ret)
{
    switch (ret)
    {
     case OK:
        QMessageBox::information(this, "注册", "注册成功");
        break;
    case ERROR:
        QMessageBox::information(this, "注册", "注册失败,发生未知错误");
        break;
    case ERROR_EXIST:
        QMessageBox::information(this, "注册", "注册失败,用户已存在");
        break;
     default:
        break;
    }
}
//返回首页
void Register::on_ReturnButton_clicked()
{
    this->close();
}

