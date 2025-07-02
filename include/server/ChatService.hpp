#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <memory>
#include <mutex>

#include "json.hpp"
#include "UserModel.hpp"
#include "OfflineMsgModel.hpp"
#include "FriendModel.hpp"
#include "GroupModel.hpp"


using namespace std;
using namespace muduo;
using namespace muduo::net;
using json=nlohmann::json;

using MsgHandler =std::function<void(const TcpConnectionPtr& conn,json &js,Timestamp )>;


//聊天服务器业务类
class ChatService
{
private:
    ChatService();
    //id和其对应处理器
    unordered_map<int,MsgHandler> msgHandlerMap;

    //在线用户的连接
    unordered_map<int,TcpConnectionPtr>userConnMap;

    //互斥锁
    mutex connMutex;

    //数据操作类对象
    UserModel userModel;
    FriendModel friendModel;
    OfflineMsgModel offlinemsgModel;//离线消息
    GroupModel groupModel;

public:
    //获取单例对象
    static ChatService * instance();

    //处理登录业务
    void login(const TcpConnectionPtr &conn,json &js,Timestamp time);

    //处理注册业务
    void reg(const TcpConnectionPtr &conn,json &js,Timestamp time);

    //一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn,json &js,Timestamp time);

    //添加好友
    void addFriend(const TcpConnectionPtr &conn,json &js,Timestamp time);

    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);

    //客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    //服务器异常结束后，重置
    void reset();

    void createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);

    void addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);

    void groupChat(const TcpConnectionPtr &conn,json &js,Timestamp time);

    void loginout(const TcpConnectionPtr &conn,json &js,Timestamp time);
};






#endif