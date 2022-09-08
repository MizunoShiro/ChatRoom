#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <QKeyEvent>
#include "register.h"
#include "communicate.h"
#include "brainstorm.h"
#include "root.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();   
    void keyPressEvent(QKeyEvent *e);

private slots:
    void on_LoginpushButton_clicked();

    void on_RegisterButton_clicked();

    void rstLogin(QJsonObject &Json);   //处理登录结果

    void rstRootLogin(QJsonObject &Json);    //处理管理员登录结果

    void on_LoginButton_root_clicked();

private:
    Ui::Widget *ui;

    Communicate *_com;  //通信类指针，负责和服务器通信
};

#endif // WIDGET_H
