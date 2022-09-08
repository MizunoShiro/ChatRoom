#include "root.h"
#include "ui_root.h"

Root::Root(Communicate *com, QJsonObject &Json, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Root)
{
    ui->setupUi(this);
    this->setWindowTitle("管理员");
    _com = com;
    initOnline(Json);
    connect(_com, &Communicate::onlineRefresh, this, &Root::onlineRefresh);
    this->setWindowIcon(QIcon("D:/MinGW/2022C++/BrainStorm/advertisement/brainstorm.webp"));
}

Root::~Root()
{
    delete ui;
}

//初始化在线人数列表
void Root::initOnline(QJsonObject &Json)
{
    int size = Json["name"].toArray().size();
    char text[BUF_SIZE];
    QTableWidgetItem *name[size];
    QTableWidgetItem *rank[size];
    ui->tableWidget->setRowCount(size);
    ui->tableWidget->setColumnCount(2);
    QStringList header;
    header << "账号" << "段位";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->resizeSection(0,200); //设置表头第1列的宽度为180
    //不允许编辑表格
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sprintf(text,"在线用户列表(当前在线用户%d人)",size);
    ui->onLinelabel->setText(text);
    for(int i = 0; i < size; i++)
    {
        name[i] = new QTableWidgetItem(Json["name"].toArray().at(i).toString());
        name[i]->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->setItem(i, 0, name[i]);

        rank[i] = new QTableWidgetItem(Json["rank"].toArray().at(i).toString());
        rank[i]->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->setItem(i, 1, rank[i]);
    }
}

void Root::on_RefreshButton_clicked()
{
    QJsonObject Json;
    Json["cmd"] = REFRESH;
    _com->writeData(Json);
}

//刷新在线人数列表
void Root::onlineRefresh(QJsonObject &Json)
{
    int size = Json["name"].toArray().size();
    char text[BUF_SIZE];
    QTableWidgetItem *name[size];
    QTableWidgetItem *rank[size];
    ui->tableWidget->setRowCount(size);
    sprintf(text,"在线用户列表(当前在线用户%d人)",size);
    ui->onLinelabel->setText(text);
    for(int i = 0; i < size; i++)
    {
        name[i] = new QTableWidgetItem(Json["name"].toArray().at(i).toString());
        name[i]->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->setItem(i, 0, name[i]);

        rank[i] = new QTableWidgetItem(Json["rank"].toArray().at(i).toString());
        rank[i]->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->setItem(i, 1, rank[i]);
    }
}


void Root::on_PutPicButton_clicked()
{
    _upLoad = new QFileDialog(this);
    QString filename = _upLoad->getOpenFileName(this, "请选择图片", ".", QStringLiteral("图片文件(*jpg)"));

    delete _upLoad;
    _upLoad = NULL;
    this->hide();
    ImageTransfer it(_com, filename);
    it.exec();
    this->show();
}
