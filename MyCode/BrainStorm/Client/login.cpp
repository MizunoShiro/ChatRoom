#include "login.h"
#include "ui_login.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle(" ");
    _com = new Communicate;
    connect(_com, SIGNAL(rstLogin(QJsonObject &)), this, SLOT(rstLogin(QJsonObject &)));
    connect(_com, SIGNAL(rstRootLogin(QJsonObject&)), this, SLOT(rstRootLogin(QJsonObject&)));
    this->setWindowIcon(QIcon("D:/MinGW/2022C++/BrainStorm/advertisement/brainstorm.webp"));
}

Widget::~Widget()
{
    delete _com;
    _com = NULL;
    delete ui;
}
void Widget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        if(ui->tabWidget->currentIndex() == 0)
        {
            on_LoginpushButton_clicked();
        }
        else if(ui->tabWidget->currentIndex() == 1)
        {
            on_LoginButton_root_clicked();
        }

    }
}
void Widget::on_LoginpushButton_clicked()
{
    QString userName = ui->NamelineEdit->text();
    QString passwd = ui->PasswordlineEdit->text();

    //将登录信息发送给服务器
    QJsonObject json;
    json["cmd"]      = LOGIN;
    json["username"] = userName;
    json["password"] = passwd;
    _com->writeData(json);
}
//处理登陆结果
void Widget::rstLogin(QJsonObject &Json)
{
    int result = Json["result"].toInt();
    switch (result)
    {
     case OK:
        {
            this->hide();
            BrainStorm *dlg = new BrainStorm(_com, Json);
            dlg->show();
            dlg->setAttribute(Qt::WA_DeleteOnClose);
            break;
        }
    case ERROR:
        QMessageBox::critical(this, "登录", "登录失败,未知错误");
        break;
    case ERROR_LOGIN:
        QMessageBox::critical(this, "登录", "登录失败,账号已登录");
        break;
    case NAMEORPASSWD:
        QMessageBox::critical(this, "登录", "登录失败,用户名或密码错误");
        break;
     default:
        break;
    }
}
//注册
void Widget::on_RegisterButton_clicked()
{
    this->hide();
    Register reg(_com);
    reg.exec();
    this->show();
}


void Widget::on_LoginButton_root_clicked()
{
    QString rootName = ui->NamelineEdit_root->text();
    QString passwd = ui->PasswordlineEdit_root->text();

    //将登录信息发送给服务器
    QJsonObject json;
    json["cmd"]      = ROOTLOGIN;
    json["username"] = rootName;
    json["password"] = passwd;
    _com->writeData(json);
}

void Widget::rstRootLogin(QJsonObject &Json)
{
    int result = Json["result"].toInt();
    switch (result)
    {
     case OK:
        {
            this->hide();
            Root *root = new Root(_com, Json);
            root->show();
            root->setAttribute(Qt::WA_DeleteOnClose);
            break;
        }
    case ERROR_LOGIN:
        QMessageBox::critical(this, "管理员登录", "登录失败,账号已登录");
        break;
    case NAMEORPASSWD:
        QMessageBox::critical(this, "管理员登录", "登录失败,用户名或密码错误");
        break;
     default:
        break;
    }
}
