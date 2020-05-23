#include "xcom_task.h"
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <iostream>
#include <string.h>
using namespace std;


static void SReadCB(struct bufferevent *bev, void *ctx)
{
    auto task = (XComTask*)ctx;
    task->ReadCB();
}
static void SWriteCB(struct bufferevent *bev, void *ctx)
{
    auto task = (XComTask*)ctx;
    task->WriteCB();
}

static void SEventCB(struct bufferevent *bev, short what,void *ctx)
{
    auto task = (XComTask*)ctx;
    task->EventCB(what);
    
}
void XComTask::Close()
{
    if(bev_)
        bufferevent_free(bev_);
    bev_ = NULL;
    if(msg_.data)
        delete msg_.data;
    memset(&msg_, 0, sizeof(msg_));
    delete this;
}
bool  XComTask::Write(const void *data, int size)
{
    if (!bev_  || !data || size <= 0)return false;
    int re = bufferevent_write(bev_, data, size);
    if (re != 0)return false;
    return true;
}
bool XComTask::Write(const XMsg *msg)
{
    if (!bev_ || !msg || !msg->data || msg->size <= 0)return false;
    //1 д����Ϣͷ
    int re = bufferevent_write(bev_, msg, sizeof(XMsgHead));
    if (re != 0)return false;
    //2 д����Ϣ����
    re = bufferevent_write(bev_, msg->data, msg->size);
    if (re != 0)return false;
    return true;
}
//����д��ص�
void XComTask::BeginWrite()
{
    if (!bev_)return;
    bufferevent_trigger(bev_, EV_WRITE, 0);
}
//bool XComTask::ReadCB(const XMsg *msg)
//{
//    cout << "recv Msg type=" << msg->type << " size=" << msg->size << endl;
//    return true;
//}
void XComTask::ReadCB()
{
    
    for (;;)//ȷ����Ե�������ܶ�ȡ��������
    {
        if (!is_recv_msg_)
        {
            int len = bufferevent_read(bev_, read_buf_, sizeof(read_buf_));
            if (len <= 0)return;
            ReadCB(read_buf_, len);
            continue;
        }
        // ������ϢXMsgHead
        //1 ����ͷ����Ϣ
        if (!msg_.data)
        {
            int headsize = sizeof(XMsgHead);
            int len = bufferevent_read(bev_, &msg_,headsize);
            if (len <= 0)return;
            if (len != headsize)
            {
                cerr << "msg head recv error" << endl;
                return;
            }
            //��֤��Ϣͷ����Ч��
            if (msg_.type >= MSG_MAX_TYPE || msg_.size <= 0 || msg_.size> MSG_MAX_SIZE)
            {
                cerr << "msg head is error" << endl;
                return;
            }
            msg_.data = new char[msg_.size];
        }
        int readsize = msg_.size - msg_.recved;
        if (readsize <= 0)
        {
            delete msg_.data;
            memset(&msg_, 0, sizeof(msg_));
            return;
        }
        int len = bufferevent_read(bev_, msg_.data+ msg_.recved, readsize);
        if (len <= 0)
            return;
        msg_.recved += len;
        if (msg_.recved == msg_.size)
        {
            //������Ϣ
            cout << "recved msg " << msg_.size << endl;
            //���������������bev_��������
            if (!ReadCB(&msg_))
            {
                return;
            }
            delete msg_.data;
            memset(&msg_, 0, sizeof(msg_));
         
        }
    }
}

void XComTask::EventCB(short what)
{
    cout << "SEventCB:" << what << endl;
    if (what & BEV_EVENT_CONNECTED)
    {
        cout << "BEV_EVENT_CONNECTED" << endl;
        //֪ͨ���ӳɹ�

        ConnectedCB();
    }

    ///�˳�Ҫ����������
    if (what & BEV_EVENT_ERROR || what & BEV_EVENT_TIMEOUT)
    {
        cout << "BEV_EVENT_ERROR or BEV_EVENT_TIMEOUT" << endl;
        Close();
    }
    if (what & BEV_EVENT_EOF)
    {
        cout << "BEV_EVENT_EOF" << endl;
        Close();
    }
}
bool XComTask::Init()
{
    //���ַ���˻��ǿͻ���
    int comsock = this->sock();
    if (comsock <= 0)
        comsock = -1;
    ///��bufferevent �������� 
    /// ����bufferevent������ -1�Զ�����socket
    bev_ = bufferevent_socket_new(base(), comsock, BEV_OPT_CLOSE_ON_FREE);
    if (!bev_)
    {
        cerr << "bufferevent_socket_new failed!" << endl;
        return false;
    }

    //���ûص�����
    bufferevent_setcb(bev_, SReadCB, SWriteCB, SEventCB, this);
    bufferevent_enable(bev_, EV_READ|EV_WRITE);

    timeval tv = { 10,0 };
    bufferevent_set_timeouts(bev_, &tv, &tv);

    //���ӷ�����
    if (server_ip_.empty())
    {
        return true;
    }
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port_);
    evutil_inet_pton(AF_INET, server_ip_.c_str(), &sin.sin_addr.s_addr);
    int re = bufferevent_socket_connect(bev_, (sockaddr*)&sin, sizeof(sin));
    if (re != 0)
        return false;
    return true;
}
