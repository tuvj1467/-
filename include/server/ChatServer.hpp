#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    ChatServer(EventLoop*eventloop,const InetAddress& listenAddr,const string & nameArg);


    void start();

    
private:

    void onConnection(const TcpConnectionPtr&);
   

    void onMessage(const TcpConnectionPtr&,Buffer*,Timestamp);
    
TcpServer m_server;
EventLoop *m_loop;


};





#endif











