#include "GroupModel.hpp"
#include "db.hpp"


bool GroupModel::createGroup(Group& group)
{
     //组装sql语句
     char sql[1024]={};
     sprintf(sql,"insert into AllGroup(groupname,groupdesc)values('%s','%s')",
             group.getName().c_str(),group.getDesc().c_str());
 
     MySQL mysql;
     if(mysql.connect())
     {
         if(mysql.update(sql))
         {
             group.setId(mysql_insert_id(mysql.getConnection()));//返回的id在创建业务中会使用
             return true;
         }
     }
 
     return false;
}


void GroupModel::addGroup(int groupid,int userid,string role)
{
     //组装sql语句
     char sql[1024]={};
     sprintf(sql,"insert into GroupUser values(%d,%d,'%s')",
        groupid,userid,role.c_str());
 
     MySQL mysql;
     if(mysql.connect())
     {
         mysql.update(sql);
         
     }
 
     
}


vector<Group> GroupModel::queryGroups(int userid)
{

    /*
    1.先根据userid在GroupUser中查询出该用户所属群组消息  
    2.根据该群组信息，查询该群组所有用户userid，并且和user表进行多表联合查询，获取用户详细信息
    */

     //组装sql语句
     char sql[1024]={};
     sprintf(sql,"select a.id,a.groupname,a.groupdesc from AllGroup a inner join \
            GroupUser b on a.id=b.groupid where b.userid=%d",
            userid);
 

    vector<Group>groupVec;


    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES*res =mysql.query(sql);

        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
                
            }
            mysql_free_result(res);
        }
                
    }

    //查询群组用户消息
    for(Group& group:groupVec)
    {
        sprintf(sql,"select a.id,a.name,a.state,b.grouprole from User a inner join \
            GroupUser b on b.userid=a.id where b.groupid=%d",group.getId());


    MYSQL_RES*res =mysql.query(sql);


    

        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
                
            }
            mysql_free_result(res);
        }
                    
    }

    return groupVec;
     
}


vector<int> GroupModel::queryGroupUsers(int userid,int groupid)
{

    //组装sql语句
    char sql[1024]={};
    sprintf(sql,"select userid from GroupUser where groupid=%d and userid!=%d",groupid,userid);
            

    vector<int>idVec;

    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES*res =mysql.query(sql);

        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                
                idVec.push_back(atoi(row[0]));
                
            }
            mysql_free_result(res);
        }
                
    }


    return idVec;
     
}


