#ifndef XCOM_TASK_H
#define XCOM_TASK_H
#include "xtask.h"
#include "xmsg.h"
#include <string>
class XCOM_API XComTask : public XTask
{
public:
    ///初始化bufferevent，客户端建立连接
    virtual bool Init();
    virtual void Close();

    void set_server_ip(std::string ip) { this->server_ip_ = ip; }
    void set_port(int port) { this->server_port_ = port; }

    ///事件回调函数
    virtual void EventCB(short what);

    //激活写入回调
    virtual void BeginWrite();

    //发送消息
    virtual bool Write(const XMsg *msg);
    virtual bool Write(const void *data, int size);

    //连接成功的消息回调，由业务类重载
    virtual void ConnectedCB() {};

    //当关闭消息接收时，数据将发送到此函数，由业务模块重载
    virtual void ReadCB(void *data, int size) {}

    //接收到消息的回调，由业务类重载 返回true正常，
    //返回false退出当前的消息处理,不处理下一条消息
    virtual bool ReadCB(const XMsg *msg) = 0;

    ///写入数据回调函数
    virtual void WriteCB() {};

    ///读取数据回调函数
    virtual void ReadCB();

    void set_is_recv_msg(bool isrecv) { this->is_recv_msg_ = isrecv; }
protected:

    //读取缓存
    char read_buf_[4096] = { 0 };
private:
    /// 服务器IP
    std::string server_ip_ = "";

    ///服务器端口
    int server_port_ = 0;

    struct bufferevent *bev_ = 0;

    //数据包缓存
    XMsg msg_;

    //是否接收消息，接收的消息会调用 void ReadCB(const XMsg *msg)
    //不接收消息调用 void ReadCB(void *data, int size)
    bool is_recv_msg_ = true;
};

#endif