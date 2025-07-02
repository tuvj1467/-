#include "ChatService.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <string>
#include <vector>
#include <map>


using namespace muduo;
using namespace std;


//获取单例对象
ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

//绑定事件对应的回调
ChatService::ChatService()
{
    // 用户基本业务管理相关事件处理回调注册
    msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)});
    msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)});
    msgHandlerMap.insert({ONE_CHAT_MSG,std::bind(&ChatService::oneChat,this,_1,_2,_3)});
    msgHandlerMap.insert({ADD_FRIEND_MSG,std::bind(&ChatService::addFriend,this,_1,_2,_3)});

    // 群组业务管理相关事件处理回调注册
    msgHandlerMap.insert({CREAT_GROUP_MSG,std::bind(&ChatService::createGroup,this,_1,_2,_3)});
    msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

    //
    msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)});



    // 连接redis服务器
    if (m_redis.connect())
    {
        // 设置上报消息的回调
        m_redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }

}

//获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    auto it=msgHandlerMap.find(msgid);
    if(it==msgHandlerMap.end())
    {
        
        return [msgid](const TcpConnectionPtr &con,json &js,Timestamp){
            LOG_ERROR<<"msgid"<<msgid<<"can not find handler!";
        };
        
    }else
    {
        return msgHandlerMap[msgid];
    }
}


void ChatService::reset( )
{
    userModel.resetState();
}



//处理登录业务
void ChatService::login(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int id=js["id"].get<int>();
    string password=js["password"];

    User user=userModel.query(id);
    if(user.getId()==id&&user.getPassword()==password)
    {   
        if(user.getState()=="online")
        {
            //已登录,不许重复登录
            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errno"]=2;
            response["errmsg"]="该账号已登录";
            conn->send(response.dump());
        }else
        {   

            //登录成功 更新状态信息
            user.setState("online");
            userModel.updateState(user);

            //登录成功 记录用户连接
            {
                lock_guard<mutex>locker(connMutex);
                userConnMap.insert({id,conn});
            }

            // id用户登录成功后，向redis订阅channel(id)
            m_redis.subscribe(id); 
    
            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errno"]=0;
            response["id"]=user.getId();
            response["name"]=user.getName();

            //该用户是否有离线消息
            vector<string>vec=offlinemsgModel.query(id);

            if(!vec.empty())
            {
                response["offlinemsg"]=vec;
                //读取完毕，将离线消息从数据库中删除
                offlinemsgModel.remove(id);
            }
            //查询用户的好友信息并返回
            vector<User>friendVec=friendModel.query(id);
            if(!friendVec.empty())
            {
                vector<string>vec2;
                for (User &user:friendVec)
                {
                    json js;
                    js["id"]=user.getId();
                    js["name"]=user.getName();
                    js["state"]=user.getState();
                    vec2.push_back(js.dump());
                }
                response["friend"]=vec2;
            }


            // 查询用户的群组信息
            vector<Group> groupuserVec = groupModel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["user"] = userV;
                    groupV.push_back(grpjson.dump());
                }

                response["groups"] = groupV;
            }



            conn->send(response.dump());
        }

    }else{
        //登录失败
        json response;
        response["msgid"]=LOGIN_MSG_ACK;
        response["errno"]=1;
        
        conn->send(response.dump());

    }

}

//处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    string name=js["name"];
    string password=js["password"];

    User user;
    user.setName(name);
    user.setPassword(password);
    bool state=userModel.insert(user);
    if(state)
    {   //注册成功 
        json response;
        response["msgid"]=REG_MSG_ACK;
        response["errno"]=0;
        response["id"]=user.getId();

        conn->send(response.dump());

    }else{
        //注册失败
        json response;
        response["msgid"]=REG_MSG_ACK;
        response["errno"]=1;

        conn->send(response.dump());

    }

}

// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();

    {
        lock_guard<mutex> lock(connMutex);
        auto it = userConnMap.find(userid);
        if (it != userConnMap.end())
        {
            userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    m_redis.unsubscribe(userid); 

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    userModel.updateState(user);
}


//客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
        {
            lock_guard<mutex>locker(connMutex);
            for(auto it=userConnMap.begin();it!=userConnMap.end();++it)
            {
                if(it->second==conn)
                {
                    user.setId(it->first);
                    //从表中删除连接
                    userConnMap.erase(it);
                    break;
                }
            }
        }   

        // 用户注销，相当于就是下线，在redis中取消订阅通道
        m_redis.unsubscribe(user.getId()); 


    //更新在线状态
    if(user.getId()!=-1)
    { 
        user.setState("offline");
        userModel.updateState(user);
    }
}


//一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int toid=js["toid"].get<int>();

    //标识用户是否在线


    {
        lock_guard<mutex>locker(connMutex);
        auto it =userConnMap.find(toid);
        if(it!=userConnMap.end())
        {
            //toid在线 转发消息 服务器主动推送消息给toid
            it->second->send(js.dump());
            
            return;
        }
    }

    // 查询toid是否在线 
    User user = userModel.query(toid);
    if (user.getState() == "online")
    {
        m_redis.publish(toid, js.dump());
        return;
    }

    
   
        //toid不在线 存储离线消息   
        offlinemsgModel.insert(toid,js.dump());

}

 //添加好友 msgid id friendid
void ChatService::addFriend(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid=js["id"].get<int>();
    int friendid=js["friendid"].get<int>();

    friendModel.insert(userid,friendid);

}



void ChatService::createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid=js["id"].get<int>();
    string name=js["groupname"];
    string desc=js["groupdesc"];

    Group group(-1,name,desc);
    if(groupModel.createGroup(group))
    {
        //存储群组创建人信息
        groupModel.addGroup(userid,group.getId(),"creator");
    }
}

//加入 群组
void ChatService::addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid=js["id"].get<int>();
    int groupid=js["groupid"].get<int>();
    groupModel.addGroup(groupid,userid,"normal");
}


//群组聊天
void ChatService::groupChat(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid=js["id"].get<int>();
    int groupid=js["groupid"].get<int>();
    vector<int>useridVec=groupModel.queryGroupUsers(userid,groupid);

    lock_guard<mutex> lock(connMutex);
    for(int id:useridVec)
    {
        auto it=userConnMap.find(id);
        if(it!=userConnMap.end())
        {
            //转发群消息
            it->second->send(js.dump());
        }
        else{
             // 查询toid是否在线 
             User user = userModel.query(id);
             if (user.getState() == "online")
             {
                m_redis.publish(id, js.dump());
             }
             else
             {
                // 存储离线群消息
                offlinemsgModel.insert(id, js.dump());
             }
        }
    }

}


// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(connMutex);
    auto it = userConnMap.find(userid);
    if (it != userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    offlinemsgModel.insert(userid, msg);
}


