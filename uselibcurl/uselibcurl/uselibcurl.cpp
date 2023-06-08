// uselibcurl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "curl.h"
#include "json.h"
#include <string>
#include <stdio.h>
#include <map>

#define URL "127.0.0.1:80"
//define URL "127.0.0.1:80/jpg"

//头部信息键值对
std::map<std::string, std::string> mHead;


int offset = 0;

std::string find_header(const std::string &str)
{
	if (mHead.find(str) != mHead.end())
	{
		return mHead[str];
	}
	else
	{
		return nullptr;
	}

}

//server 端响应正文回调函数
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) 
{
	offset = offset + size * nmemb;
	std::string  str = find_header("Content-Type");
	//图片 就写文件
	if (str.find("image") != -1)
	{
		str = find_header("Content-Length");
		int file_len = atoi(str.c_str());
		FILE *fp = (FILE *)userdata;
		int ret = fwrite(ptr, size, nmemb, fp);
		double percent = (double)offset / (double)file_len;
		printf("%f\r\n", percent * 100);
	}
	//text 就打印输出

	if (str.find("text") != -1)
	{
		std::cout << ptr << std::endl;
	}
	return nmemb;
}

//server 端响应头 回调函数
size_t WriteHeadCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::string pBuffer , key , value;
	size_t length = size * nmemb;
	pBuffer.append((char*)ptr, length);
	int pos = pBuffer.find(": ");
	key = pBuffer.substr(0, pos);
	if (pos == -1)
	{
		value = "";
	}
	else
	{
		//length - pos - 2 - 2  ": " 占两个字节，"\r\n"占两个字节
		value = pBuffer.substr(pos + 2, length - pos - 2 - 2);
	}
	mHead.insert({ key,value });
	return nmemb;
}
int main()
{
	//初始化Curl：
	curl_global_init(CURL_GLOBAL_ALL);

	// 构建一个Curl对象：

	CURL * curl = curl_easy_init();
	if (!curl)
	{
		std::cout << "intit fail!" << std::endl;
	}

	// 设置请求URL：
	curl_easy_setopt(curl, CURLOPT_URL, URL);

	//设置请求方式post：
	curl_easy_setopt(curl, CURLOPT_POST, 1L); // POST请求

	//设置请求头：

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	//设置json数据
	Json::Value val;
	val["name"] = "neilyo";
	//将json转string
	Json::FastWriter fast_writer;
	std::string str = fast_writer.write(val);
	//设置传输的字符串
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, str.length());

	//设置响应正文回调函数
	FILE *fp;
	fopen_s(&fp, "data.jpg", "ab+");
	fseek(fp, 0, SEEK_SET);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	//设置相应头回调函数
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteHeadCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, nullptr);



	CURLcode ret = curl_easy_perform(curl);

	if (CURLE_OK != ret)
	{
		std::cout << "send fail msg is:" << curl_easy_strerror(ret) <<std::endl;
	}
	else
	{
		std::cout << "send ok " << std::endl;
	}

	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	fclose(fp);
	getchar();

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
