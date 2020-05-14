#include <event2/event.h>
#include <string.h>
#include <iostream>
#ifdef _WIN32
#include<ws2tcpip.h>
#else
#include <signal.h>
#endif
using namespace std;

void ListenCB(evutil_socket_t sock, short what, void *arg)
{
    cout << "ListenCB" << endl;
    if (!(what & EV_READ))
    {
        cout << "not read" << endl;
        return;
    }
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    socklen_t size = sizeof(sin);
    evutil_socket_t client_socket = accept(sock, (sockaddr*)&sin, &size);
    if (client_socket <= 0)
    {
        cerr << "accept error" << endl;
        return;
    }
    char ip[16] = { 0 };
    evutil_inet_ntop(AF_INET, &sin.sin_addr,ip, sizeof(ip));
    cout << "client ip is " << ip << endl;

}


int main(int argc,char *argv[])
{
    int server_port = 20010;
    if (argc > 1)
    {
        server_port = atoi(argv[1]);
    }
#ifdef _WIN32 
	//初始化socket库
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
#else
    //使用断开连接socket，会发出此信号，造成程序退出
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        return 1;
#endif

    std::cout << "test event server!\n"; 
	//1 创建libevent的上下文 默认是创建base锁
	event_base * base = event_base_new();
	if (base)
	{
		cout << "event_base_new success!" << endl;
	}

    //2 创建socket 绑定端口
    evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock <= 0)
    {
        // _CRT_SECURE_NO_WARNINGS
        cerr << "socket error" << strerror(errno) << endl;
        return -1;
    }
    //设置地址复用和非阻塞
    evutil_make_socket_nonblocking(sock);
    evutil_make_listen_socket_reuseable(sock);
    //绑定端口
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port);
    int re = ::bind(sock, (sockaddr*)&sin, sizeof(sin));
    if (re != 0)
    {
        // _CRT_SECURE_NO_WARNINGS
        cerr << "bind port:"<<server_port<<" error" << strerror(errno) << endl;
        return -1;
    }
    listen(sock, 10);
    cout << "bind port success!" << server_port << endl;

    //3 注册socket的监听事件回调函数 EV_PERSIST持久化，不然只进入一次事件
    // EV_ET（边缘触发） 默认水平触发（只要由数据没有处理，就一直触发）
    // event_self_cbarg() 传递当前创建的event对象
    event *ev = event_new(base, sock, EV_READ | EV_PERSIST, ListenCB, event_self_cbarg());
    //开始监听事件  第二个参数，超时时间
    event_add(ev, 0);
    

    /// 事件主循环 监控事件是否发送， 分发事件到回调函数
    /// 如果没有事件注册则退出
    event_base_dispatch(base);
    evutil_closesocket(sock);
    event_del(ev);
    event_free(ev);
    event_base_free(base);


	return 0;
}
