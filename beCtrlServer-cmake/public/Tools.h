#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>     // 信号头文件
#include <sys/stat.h>   // umask函数头文件
#include <map>
#include <curl/curl.h>  // curl头文件
#include <unistd.h>

class CTools
{
public:
	// 创建守护进程
	static int SwitchDeamon()
	{
		pid_t ret = fork();
		if (ret == -1)return -1;
		if (ret > 0) {
			// 主进程
			exit(0);
		}
		// 子进程
		ret = setsid();
		if (ret == -1)return -2;
		ret = fork();
		if (ret == -1)return -3;
		if (ret > 0) {
			// 子进程
			exit(0);
		}
		// 孙进程
		for (int i = 0; i < 3; i++) {
			close(i);
		}
		umask(0);
		signal(SIGCHLD, SIG_IGN);
		return 0;
	}
	/// <summary>
	/// 设置端口复用
	/// </summary>
	/// <param name="sock">要设置端口复用的套接字</param>
	/// <returns>返回值表示设置是否成功 true表示成功 false表示失败</returns>
	static bool SetPortReuse(int sock)
	{
		int optval = 1;
		int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		return ret < 0 ? false : true;
	}
	/// <summary>
	/// 发送http/https请求，通过curl
	/// </summary>
	/// <param name="host">服务地址 libcurl默认是不支持SSL的, 如果需要支持https，请在libcurl的编译选项中设置</param>
	/// <param name="uri">接口地址</param>
	/// <param name="params">请求参数字段</param>
	/// <returns>返回为未经处理的应答包</returns>
	static std::string CurlTool(
		const std::string& host,
		const std::string& uri,
		const std::map<std::string, std::string>& params,
		std::string& retUrl)
	{

		// TODO:curl发送http各种类型的请求，c++代码的方式

		std::string query_str = "";
		for (auto& i : params) {
			query_str += "&" + i.first + "=" + i.second;
		}
		query_str = uri + "?" + query_str.substr(1);
		// 发送请求
		CURL* curl;
		CURLcode result_code;
		// int error_code = 0;
		curl = curl_easy_init();
		if (curl == NULL) {
			return "";
		}
		std::string curl_result;
		std::string url = host + query_str;
		// std::cout << "url:" << url << std::endl;

		retUrl = url;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curl_result);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlTool_callback);
		result_code = curl_easy_perform(curl); // 发送请求
		if (result_code != CURLE_OK) {
			// std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(result_code) << std::endl;
			return "";
		}
		curl_easy_cleanup(curl);
		return curl_result;

		// std::cout << "返回结果为：" << std::endl;
		// std::cout << curl_result << std::endl;
	}
	/// <summary>
	/// 应用重启实现
	/// </summary>
	/// <param name="argc">程序运行时输入到命令行中的参数的个数</param>
	/// <param name="argv">每个参数的值</param>
	static void restart(int argc, char* argv[])
	{
		std::string cmd;
		char* str = NULL;
		size_t len = 0;
		for (int i = 0; i < argc; i++) {
			cmd += argv[i];
			if (i != argc - 1)
				cmd += " ";
		}
		cmd = "ps -e | grep " + cmd.substr(2);
		// const std::string& pName
		// 找到正在运行的进程的id，然后给他关闭掉
		// std::string cmd = "ps -e | grep " + pName.substr(2);    // "ps -aux | grep " + pName.substr(2);  // 
		pid_t pid = getpid(), oldPid = 0;
		// printf("cmd: [%s]  当前进程的id：%d\n", cmd.c_str(), pid);
		FILE* pFile = popen(cmd.c_str(), "r");
		if (pFile == NULL) {
			printf("<%s>(%d):命令行管道打开失败\n", __FILE__, __LINE__);
			exit(0);
		}
		bool oldClear = false;
		while (true)
		{
			ssize_t ret = getline(&str, &len, pFile);
			if (ret < 0) {
				break;
			}
			ret = sscanf(str, "%d", &oldPid);
			if (ret <= 0) {
				printf("<%s>(%d):读取错误\n", __FILE__, __LINE__);
			}
			// printf("[%s] 导出的pid: %d\n", str, oldPid);
			if (oldPid != pid) {
				// 这个不是当前运行的进程
				// printf("不是当前运行的进程\n");
				kill(oldPid, SIGINT);
				oldClear = true;
			}
			/*else {
				// 这个是当前运行的进程
				printf("是当前运行的进程\n");
			}*/
		}
		pclose(pFile);
		if (oldClear) {
			printf("[info]:已将正在运行的上一版本进程结束\n");
		}
	}
private:
	/// <summary>
	/// curl发送http请求调用的回调函数，回调函数中对返回的json格式的body进行了解析，解析结果储存在result中
	/// </summary>
	/// <param name="ptr">参数定义见libcurl库文档</param>
	/// <param name="size">参数定义见libcurl库文档</param>
	/// <param name="nmemb">参数定义见libcurl库文档</param>
	/// <param name="stream">参数定义见libcurl库文档</param>
	/// <returns>返回值定义见libcurl库文档</returns>
	static size_t CurlTool_callback(void* ptr, size_t size, size_t nmemb, void* stream) {
		// 获取到的http body存放在ptr中，先将其转换为string格式
		std::string s((char*)ptr, size * nmemb);
		std::string* result = static_cast<std::string*>(stream);
		*result += s;
		return size * nmemb;
	}
};
