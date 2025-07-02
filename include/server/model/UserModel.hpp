#ifndef USERMODEL_H
#define USERMODEL_H


#include "User.hpp"

//User表的数据操作类
class UserModel
{
public:
    bool insert(User &user);
    
    User query(int id);

    bool updateState(User uaer);

    //重置用户状态
    void resetState();
};










#endif






