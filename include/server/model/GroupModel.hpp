#ifndef GROUPMODEL_H
#define GROUPMODEL_H


#include "Group.hpp"
#include <string>
#include <vector>

using namespace std;

class GroupModel
{
public:
    //创建群组
    bool createGroup(Group& group);

    //加入 群组
    void addGroup(int userid,int groupid,string role);

    //查询用户所在的群组信息 
    vector<Group>queryGroups(int userid);

    //根据指定的groupid查询群组用户id
    vector<int>queryGroupUsers(int userid,int groupid);


};


#endif