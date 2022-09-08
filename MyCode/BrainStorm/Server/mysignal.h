#ifndef MYSIGNAL_H
#define MYSIGNAL_H

//用户操作指令    5xxx
#define OK                5000
#define REGISTER          5001       //注册
#define LOGIN             5002       //登录
#define ROOTLOGIN         5003       //管理员登录
#define ALONE_GETQUESTION 5004       //个人训练获取题目
#define RANK              5005       //开始排位
#define ANSWER            5006       //答题信息
#define RANKRESULT        5007       //答题结果
#define REFRESH           5008       //刷新在线列表
#define IMAGE             5009       //发送图片

//错误    8xxx
#define ERROR        8001    //未知错误
#define ERROR_EXIST  8002    //用户已存在，重复创建
#define NAMEORPASSWD 8003    //用户名或密码错误
#define ERROR_LOGIN  8004    //用户已登录，重复登录
#define OFFLINE      8005    //用户对手掉线

//其他
#define QUESTION_NUM 5  //题目数量
#define SQL_SIZE 128    //数据库语句
#define BUF_SIZE 1024   //字符串
#define TRANSFER 4096   //一次传输文件的大小

#endif // SINGAL_H
