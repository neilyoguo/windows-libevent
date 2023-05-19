// testlibevent.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <winsock2.h>
#include <event.h>
#include <event2/listener.h>
#pragma comment (lib, "ws2_32.lib")  


void read_cb(struct bufferevent *bev, void *ctx)
{
	struct evbuffer *input = bufferevent_get_input(bev);
	//读区缓冲区数据，直到读完为止
	while (evbuffer_get_length(input))
	{
		char buf[4] = { 0 };
		int ret = bufferevent_read(bev, buf, sizeof(buf));
		if (ret < 0)
		{
			std::cout << "read error" << std::endl;
		}
		else
		{
			std::cout << "msg is :" << buf << std::endl;
		}
	}
	const char str[] = "helloword";
	bufferevent_write(bev, str, strlen(str));
}
void write_cb(struct bufferevent *bev, void *ctx)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	//获取缓冲区大小，如果还有数据，将将数据刷新到网络中
	if (evbuffer_get_length(output))
	{
		bufferevent_flush(bev, EV_WRITE, BEV_NORMAL);
		std::cout << "flush data form buf to net" << std::endl;
	}
}
void event_cb(struct bufferevent *bev, short events, void *ctx)
{
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
	{
		std::cout << "client exit" << std::endl;
	}
	else
	{
		std::cout << "other error" << std::endl;
	}
	bufferevent_free(bev);
}

void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sa, int socklen, void *user_data)
{

	struct sockaddr_in * addr = (sockaddr_in *)sa;
	char ip[16] = "";
	int port = ntohs(addr->sin_port);
	inet_ntop(AF_INET, &(addr->sin_addr), ip, sizeof(ip));
	std::cout << "client ip:" << ip << " port:" << port << std::endl;
	struct event_base *base = evconnlistener_get_base(listener);
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);

	bufferevent_enable(bev, EV_READ | EV_WRITE);


}
int main()
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	struct sockaddr_in server_addr = { 0 };
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	struct event_base *base = event_base_new();
	struct evconnlistener *listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&server_addr,
		sizeof(server_addr));

	//开始监听事件
	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_base_free(base);
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
