#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <iostream>
#ifdef _WIN32
#else
#include <signal.h>
#endif
using namespace std;


void ReadCB(struct bufferevent *bev, void *ctx)
{
	cout << "+" << flush;
	char buf[1024] = { 0 };
	int len = bufferevent_read(bev, buf, sizeof(buf) - 1);
	cout << buf << endl;
	//插入buffer链表
	bufferevent_write(bev, "OK", 3);
}

void WriteCB(struct bufferevent *bev, void *ctx)
{
	cout << "[W]" << endl;
}

void EventCB(struct bufferevent *bev, short what, void *ctx)
{
	cout << "[E]" << endl;
	//读超时
	if (what & BEV_EVENT_TIMEOUT && what &BEV_EVENT_READING)
	{
		cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << endl;
		//读取缓冲中内容

		//清理空间，关闭监听
		bufferevent_free(bev);
	}
	//写超时
	else if (what & BEV_EVENT_TIMEOUT && what &BEV_EVENT_WRITING)
	{
		cout << "BEV_EVENT_WRITING BEV_EVENT_TIMEOUT" << endl;
		//缓冲回滚

		//清理空间，关闭监听
		bufferevent_free(bev);
	}
	//异常错误
	else if (what &BEV_EVENT_ERROR)
	{
		cout << "BEV_EVENT_ERROR" << endl;
		//清理空间，关闭监听
		bufferevent_free(bev);
	}
	//连接断开
	else if (what &BEV_EVENT_EOF)
	{
		cout << "BEV_EVENT_EOF" << endl;
		//考虑缓冲的处理
		//清理空间，关闭监听
		bufferevent_free(bev);
	}
}

void ListenCB(struct evconnlistener *evc, evutil_socket_t client_socket, struct sockaddr *client_addr, int socklen, void *arg)
{
	char ip[16] = { 0 };
	sockaddr_in *addr = (sockaddr_in *)client_addr;
	evutil_inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
	cout << "client ip is " << ip << endl;
	event_base *base = (event_base *)arg;
	//创建bufferevent 上下文
	// BEV_OPT_CLOSE_ON_FREE 关闭bev是关闭socket 创建了event对象（read和wirte）
	bufferevent *bev = bufferevent_socket_new(base, client_socket, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		cerr << "bufferevent_socket_new failed!" << endl;
		return;
	}

	//添加监控事件 设置内部权限参数
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	//超时设定 秒，微秒（1/1000000秒）  读超时 和写超时
	timeval t1 = { 10,0 };
	bufferevent_set_timeouts(bev, &t1, 0);

	//设置回调函数
	bufferevent_setcb(bev, ReadCB, WriteCB, EventCB, base);

}


int main(int argc, char *argv[])
{
	int server_port = 20010;
	if (argc > 1)
	{
		server_port = atoi(argv[1]);
	}
#ifdef _WIN32 
	//初始化socket库
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
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
	//绑定端口
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(server_port);
	auto evc = evconnlistener_new_bind(base, ListenCB, base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,  ///listen back
		(sockaddr*)&sin,
		sizeof(sin)
	);


	/// 事件主循环 监控事件是否发送， 分发事件到回调函数
	/// 如果没有事件注册则退出
	event_base_dispatch(base);
	evconnlistener_free(evc);
	event_base_free(base);


	return 0;
}
