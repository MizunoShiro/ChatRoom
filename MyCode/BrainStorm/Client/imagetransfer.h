#ifndef IMAGETRANSFER_H
#define IMAGETRANSFER_H

#include <QDialog>
#include <QPixmap>
#include <QFile>
#include <QByteArray>
#include <QMessageBox>
#include <QJsonObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include "communicate.h"

namespace Ui {
class ImageTransfer;
}

class ImageTransfer : public QDialog
{
    Q_OBJECT

public:
    explicit ImageTransfer(){}
    explicit ImageTransfer(Communicate *com, QString filename,QWidget *parent = 0);
    ~ImageTransfer();
    
signals:
    //void userDownLoadAdvert();
private slots:
    void on_SureButton_clicked();

    void on_CancelButton_clicked();

    void sendData();

private:
    Ui::ImageTransfer *ui;
    QTcpSocket _s;
    QString _filename;
    Communicate *_com;
    QFile *file;
    int size;
};

#endif // IMAGETRANSFER_H
