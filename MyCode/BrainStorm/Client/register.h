#ifndef REGISTER_H
#define REGISTER_H

#include <QDialog>
#include <QJsonObject>
#include <QMessageBox>
#include <QString>
#include <QKeyEvent>
#include "communicate.h"
#include "mysignal.h"

namespace Ui {
class Register;
}

class Register : public QDialog
{
    Q_OBJECT

public:
    explicit Register(Communicate *com,QWidget *parent = 0);
    ~Register();
    void keyPressEvent(QKeyEvent *e);

private slots:
    void on_RegisterButton_clicked();

    void on_ReturnButton_clicked();

    void rstResult(int );   //处理注册结果


private:
    Ui::Register *ui;
    Communicate *_com;
};

#endif // REGISTER_H
