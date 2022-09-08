#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <mysql/mysql.h>
#include <mutex>
#include <errno.h>
#include <spdlog/spdlog.h>
#include <json/json.h>

class MyDataBase
{
public:
    MyDataBase(const char* host, const char *userName, const char *passwd, const char *dbName);

    //执行数据库语句
    bool db_exec(const char *sql);

    //数据库查询
    bool db_select(const char *sql, Json::Value &outJson);
private:
    MYSQL *_mysql;   //数据库句柄
    std::mutex _mutex; //数据库句柄互斥锁
};

#endif // MYDATABASE_H
