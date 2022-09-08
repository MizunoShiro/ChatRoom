#include "imagetransfer.h"
#include "ui_imagetransfer.h"

ImageTransfer::ImageTransfer(Communicate *com,QString filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageTransfer)
{
    ui->setupUi(this);
    _com = com;
    _filename = filename;
    QPixmap img(_filename);
    this->setWindowTitle("上传广告图片");
    ui->Imagelabel->setPixmap(img);

    connect(&_s, SIGNAL(connected()), this, SLOT(sendData()));
    connect(&_s, &QTcpSocket::readyRead, [=](){
        QByteArray data = _s.readAll();
        qDebug()<<data.toInt();
        //if(data.toInt() == 1)
        {
            QMessageBox::information(this, "提示", "传输照片完成");

            //通知user下载广告
            QJsonObject Json;

            Json["cmd"]  = IMAGE;
            Json["size"] = size;
            _com->writeData(Json);
        }
    });
}

ImageTransfer::~ImageTransfer()
{
    delete ui;
}

void ImageTransfer::on_SureButton_clicked()
{
    _s.connectToHost(QHostAddress("192.168.12.128"), 6667);
}

void ImageTransfer::sendData()
{
    int flag = 1;
    _s.write((char *)&flag, sizeof(flag));

    QMessageBox::information(this, "提示", "正在传输照片");
    file = new QFile(_filename);
    file->open(QIODevice::ReadOnly);

    QByteArray imgAry = file->readAll();
    size = imgAry.size();
    _s.write((char *)&size, sizeof(size));
    _s.write(imgAry);

    file->close(); 
    delete file;
    file = NULL;

}
void ImageTransfer::on_CancelButton_clicked()
{
    this->close();
}
