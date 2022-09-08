#ifndef ROOT_H
#define ROOT_H

#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include "communicate.h"
#include "imagetransfer.h"

namespace Ui {
class Root;
}

class Root : public QDialog
{
    Q_OBJECT

public:
    explicit Root(Communicate *com, QJsonObject &Json, QWidget *parent = 0);
    ~Root();
private slots:
    void on_RefreshButton_clicked();

    void on_PutPicButton_clicked();

    //刷新在线用户列表
    void onlineRefresh(QJsonObject &Json);

private:
    void initOnline(QJsonObject &Json);

private:
    Ui::Root *ui;
    Communicate *_com;
    QFileDialog *_upLoad;

};

#endif // ROOT_H
