#ifndef PUBLIC_H
#define PUBLIC_H



enum EnMsgTupe
{
    UNKNOW,
    LOGIN_MSG,//登录消息  1
    LOGIN_MSG_ACK,//登录响应消息  2
    REG_MSG,//注册消息  3
    REG_MSG_ACK,//注册响应消息  4
    ONE_CHAT_MSG,//聊天消息  5
    ADD_FRIEND_MSG,//添加好友 6

    CREAT_GROUP_MSG, //创建群组 7
    ADD_GROUP_MSG, //加入群组 8
    GROUP_CHAT_MSG, //群聊天 9

    LOGINOUT_MSG //注销业务 10

 
};



//{"msgid":1,"id":3,"password":"111"} 
//{"msgid":5,"id":3,"password":"111"} 
//{"msgid":1,"id":1,"password":"123456"} 
//{"msgid":5,"id":1,"from":"hansao","to":3,"msg":"hello"}
//{"msgid":5,"id":3,"from":"hh","to":1,"msg":"hello"}






#endif