#include "db.hpp"

#include <muduo/base/Logging.h>


static string server="127.0.0.1";
static string user="root";
static string password="123456";
static string dbname="chat";



MySQL::MySQL()
    {
        conn=mysql_init(nullptr);

    }

    MySQL::~MySQL()
    {
        if(conn!=nullptr)
        {
            mysql_close(conn);
        }
    }

    bool MySQL::connect()
    {
        MYSQL* p=mysql_real_connect(conn,server.c_str(),user.c_str(),
                                            password.c_str(),dbname.c_str(),3306,nullptr,0);


        if(p!=nullptr)
        {
            mysql_query(conn,"set name gbk");
            LOG_INFO<<"connect mysql success!";
        }
        else{
            LOG_INFO<<"connect mysql fail!";
        }

        return p;
    }


    bool MySQL::update(string sql)
    {
        if(mysql_query(conn,sql.c_str()))
        {
            LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<sql<<"update fail!";
            return false;
        }

        return true;
    }


    MYSQL_RES* MySQL::query(string sql)
    {
        if(mysql_query(conn,sql.c_str()))
        {
            LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<sql<<"query fail!";
            return nullptr;
        }

        return mysql_use_result(conn);
    }


    MYSQL* MySQL::getConnection()
    {
        return conn;
    }










