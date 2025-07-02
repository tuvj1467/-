#include "OfflineMsgModel.hpp"
#include "db.hpp"

//储存 用户的离线消息
void OfflineMsgModel::insert(int userid,string msg)
{
    char sql[1024]={};
    sprintf(sql,"insert into OfflineMessage values(%d,'%s')",
            userid,msg.c_str());

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
        
        return ;
        
    }

    //printf("insert into OfflineMessage faile!");
}

//删除 用户的离线消息
void OfflineMsgModel::remove(int userid)
{
    char sql[1024]={};
    sprintf(sql,"delete from OfflineMessage where userid=%d",userid);

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
        
        
    }

    //printf("insert into OfflineMessage faile!");
}

//查询 用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    char sql[1024]={};
    sprintf(sql,"select message from OfflineMessage where userid=%d",userid);

    vector<string>vec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES *res=mysql.query(sql);
        if(res!=nullptr)
        {
            //从数据库取出离线消息放入vector
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }
    }

    return vec;
}





