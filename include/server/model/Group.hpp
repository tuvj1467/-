#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include <string>
#include "GroupUser.hpp"

using namespace std;


class Group
{
public:
    Group(int id=1,string name=" ",string desc =" "):m_id(id),m_name(name),m_desc(desc)
    {}

    void setId(int id){ m_id=id; }
    void setName(string name){ m_name=name; }
    void setDesc(string desc){ m_desc=desc; }
    
    int getId(){ return m_id;}
    string getName(){ return m_name;}
    string getDesc(){ return m_desc;}

    vector<GroupUser>& getUsers(){ return this->users; }


private:
    int m_id;
    string m_name;
    string m_desc;
    vector<GroupUser>users;
};






#endif