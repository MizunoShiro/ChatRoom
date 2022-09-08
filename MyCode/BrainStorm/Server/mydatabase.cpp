
#include "mydatabase.h"

MyDataBase::MyDataBase(const char* host, const char *userName, const char *passwd, const char *dbName)
{
    //初始化数据库句柄
    _mysql = mysql_init(NULL);
    if(_mysql == NULL)
    {
        spdlog::get("BrainStorm")->error("mysql_init error!");
        exit(1);
    }

    MYSQL *con = mysql_real_connect(_mysql, host, userName, passwd, dbName, 0, NULL, 0);
    if(con == NULL)
    {
        spdlog::get("BrainStorm")->error("mysql connect error!: {}", mysql_error(_mysql));
        exit(1);
    }
    _mysql = con;

    //设置字符集
    int ret = mysql_query(_mysql, "set names utf8");
    if(ret != 0)
    {
        spdlog::get("BrainStorm")->error("mysql_query error!: {}", mysql_error(_mysql));
        exit(1);
    }
}

bool MyDataBase::db_exec(const char *sql)
{
    std::unique_lock<std::mutex> loc(_mutex);   //数据库句柄上锁
    int ret = mysql_query(_mysql, sql);
    if(ret != 0)
    {
        spdlog::get("BrainStorm")->error("mysql_query error!: {}", mysql_error(_mysql));
        return false;
    }
    return true;
}

bool MyDataBase::db_select(const char *sql, Json::Value &outJson)
{
    std::unique_lock<std::mutex> loc(_mutex);   //数据库句柄上锁
    int ret = mysql_query(_mysql, sql);
    if(ret != 0)
    {
        spdlog::get("BrainStorm")->error("mysql_query error!: {}", mysql_error(_mysql));
        return false;
    }

    //从mysql服务器下载查询结果
    MYSQL_RES *sql_res = mysql_store_result(_mysql);
    if(sql_res == NULL)
    {
        if(mysql_errno(_mysql) == 0)
        {
            return true;
        }
        else
        {
            spdlog::get("BrainStorm")->error("mysql_store_result error!: {}", mysql_error(_mysql));
        }
    }

    MYSQL_ROW row;
    unsigned int num_fields = mysql_num_fields(sql_res);  //获取查询到的列数
    MYSQL_FIELD *fetch_field = mysql_fetch_field(sql_res);     //获取表头

    while (row = mysql_fetch_row(sql_res))    //从结果中获取一行数据
    {
        for(unsigned int i = 0; i < num_fields; i++)
        {
            outJson[fetch_field[i].name].append(row[i]);
        }
    }
    mysql_free_result(sql_res);
    return true;
}
