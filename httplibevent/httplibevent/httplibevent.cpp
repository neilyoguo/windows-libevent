// httplibevent.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdlib.h>
#include <winsock2.h>
#include <event.h>
#include <event2/listener.h>
#include <string.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
using namespace std;
#pragma comment (lib, "ws2_32.lib")  


void http_cb(struct evhttp_request* req, void* arg)
{

	//获取客户端请求的URI(使用evhttp_request_uri或直接req->uri)
	const char *uri = evhttp_request_uri(req);
	printf("uri = %s\n", uri);
	const char *cmdtype;

	//获取请求类型
	switch (evhttp_request_get_command(req))
	{
	case EVHTTP_REQ_GET: cmdtype = "GET"; break;
	case EVHTTP_REQ_POST: cmdtype = "POST"; break;
	case EVHTTP_REQ_HEAD: cmdtype = "HEAD"; break;
	case EVHTTP_REQ_PUT: cmdtype = "PUT"; break;
	case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;
	case EVHTTP_REQ_OPTIONS: cmdtype = "OPTIONS"; break;
	case EVHTTP_REQ_TRACE: cmdtype = "TRACE"; break;
	case EVHTTP_REQ_CONNECT: cmdtype = "CONNECT"; break;
	case EVHTTP_REQ_PATCH: cmdtype = "PATCH"; break;
	default: cmdtype = "unknown"; break;
	}
	printf("request type  = %s \n", cmdtype);

	//遍历头部信息
	struct evkeyvalq *headers;
	struct evkeyval *header;
	headers = evhttp_request_get_input_headers(req);
	for (header = headers->tqh_first; header != NULL; header = header->next.tqe_next)
	{
		printf("%s: %s\n", header->key, header->value);
	}
	//查找头部信息
	const char *content_type = evhttp_find_header(headers, "Content-Type");
	if (content_type != NULL)
	{
		printf("Content-Type: %s\n", content_type);
	}
	else
	{
		printf("Content-Type not found\n");
	}
	//获取消息
	evbuffer* inbuf = evhttp_request_get_input_buffer(req);
	while (evbuffer_get_length(inbuf))
	{
		char buf[1024];
		int n = evbuffer_remove(inbuf, buf, sizeof(buf) - 1);
		if (n > 0)
		{
			buf[n] = '\0';
			printf("%s", buf);
		}
	}

	// 发送HTTP响应
	struct evbuffer *evbuf = evbuffer_new();
	if (evbuf == NULL)
	{
		return;
	}
	// 设置HTTP响应头
	evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/html; charset=UTF-8");
	// 构造HTTP响应内容
	const char *content = "<html><body><h1>Hello,neilyoguo</h1></body></html>";
	evbuffer_add(evbuf, content, strlen(content));
	// 发送HTTP响应
	evhttp_send_reply(req, HTTP_OK, "OK", evbuf);
	evbuffer_free(evbuf);
}

void http_jpg_cb(struct evhttp_request* req, void* arg)
{
	// 发送HTTP响应
	struct evbuffer *evbuf = evbuffer_new();
	if (evbuf == NULL)
	{
		return;
	}
	// 设置HTTP响应头
	evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "image/jpeg");
	//读取图片
	FILE *fp;
	fopen_s(&fp, "qop.jpeg", "ab+");

	if (fp == NULL)
	{
		evhttp_send_error(req, HTTP_NOTFOUND, "File Not Found");
		evbuffer_free(evbuf);
		return;
	}
	fseek(fp, 0, SEEK_END);//定位文件指针到文件尾。
	int size = ftell(fp);//获取文件指针偏移量，即文件大小

	fseek(fp, 0, SEEK_SET);
	char *buffer = (char *)malloc(size);
	size_t n = fread(buffer, 1, size, fp);

	evbuffer_add(evbuf, buffer, size);
	evhttp_send_reply(req, HTTP_OK, "OK", evbuf);
	free(buffer);
	evbuffer_free(evbuf);
	fclose(fp);//关闭文件。

}

int main()
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	//使用iocp
	struct event_config *cfg = event_config_new();
	event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);


	//初始化事件集合
	//struct event_base *base = event_init();
	struct event_base *base = event_base_new_with_config(cfg);
	//http 服务器
   //1. 创建evhttp上下文
	evhttp* evh = evhttp_new(base);

	//2. 绑定端口与ip
	if (evhttp_bind_socket(evh, "127.0.0.1", 1234) != 0) {
		std::cout << "evhttp_bind_socket failed!" << std::endl;
	}

	//3. 设定通用回调函数
	evhttp_set_gencb(evh, http_cb, NULL);
	//设置其他uri回掉函数
	evhttp_set_cb(evh, "/jpg", http_jpg_cb, NULL);
	//开始监听事件
	event_base_dispatch(base);

	evhttp_free(evh);

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
