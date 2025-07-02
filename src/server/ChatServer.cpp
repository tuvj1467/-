#include "ChatServer.hpp"
#include "json.hpp"
#include <functional>
#include <string>
#include "ChatService.hpp"

using namespace std;
using namespace placeholders;
using json=nlohmann::json;

ChatServer::ChatServer(EventLoop*loop,const InetAddress& listenAddr,const string & name)
                        :m_server(loop,listenAddr,name),m_loop(loop)
{
    m_server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));

    m_server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));

    m_server.setThreadNum(4);
}


//启动服务
void ChatServer::start()
{
    m_server.start();
}

//链接相关回调
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    //客户端断开连接
    if(!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}


//read write
void ChatServer::onMessage(const TcpConnectionPtr&conn,Buffer*buffer,Timestamp time)
{
    string buf=buffer->retrieveAllAsString();

    json js=json::parse(buf);

    auto msfHandler=ChatService::instance()->getHandler(js["msgid"].get<int>());
    msfHandler(conn,js,time);

}





