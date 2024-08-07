#pragma once
// **http模块

#include "http_parser.h"
#include "Public.h"
#include <map>

class CHttpParser
{
private:
	http_parser m_parser;
	http_parser_settings m_settings;
	std::map<Buffer, Buffer> m_HeaderValues; // 每个字段对应的值
	Buffer m_status;
	Buffer m_url;
	Buffer m_body;
	bool m_complete;
	Buffer m_lastField;
public:

	CHttpParser(enum http_parser_type type = HTTP_REQUEST);
	~CHttpParser();
	CHttpParser(const CHttpParser& http);
	CHttpParser& operator=(const CHttpParser& http);
public:
	// http数据解析
	size_t Parser(const Buffer& data);
	// GET POST ...  参考http_parser.h HTTP_METHOD_MAP宏
	unsigned Method() const {
		return m_parser.method;
	}
	// http首部包含的键值对
	const std::map<Buffer, Buffer>& Headers() const {
		return m_HeaderValues;
	}
	const Buffer& Status() const {
		return m_status;
	}
	// URL里面的变量，用键值对表示就是地址?后面的哪些变量和值
	const Buffer& Url() const {
		return m_url;
	}
	// 内容
	const Buffer& Body() const {
		return m_body;
	}
	// 错误码
	unsigned Errno() const {
		return m_parser.http_errno;
	}
protected:
	static int OnMessageBegin(http_parser* parser);
	static int OnUrl(http_parser* parser, const char* at, size_t length);
	static int OnStatus(http_parser* parser, const char* at, size_t length);
	static int OnHeaderField(http_parser* parser, const char* at, size_t length);
	static int OnHeaderValue(http_parser* parser, const char* at, size_t length);
	static int OnHeadersComplete(http_parser* parser);
	static int OnBody(http_parser* parser, const char* at, size_t length);
	static int OnMessageComplete(http_parser* parser);

	// 通过静态的构造函数，过桥到我们的成员方法上（我们初始化的时候是给了类指针的）
	int OnMessageBegin();
	int OnUrl(const char* at, size_t length);
	int OnStatus(const char* at, size_t length);
	int OnHeaderField(const char* at, size_t length);
	int OnHeaderValue(const char* at, size_t length);
	int OnHeadersComplete();
	int OnBody(const char* at, size_t length);
	int OnMessageComplete();
};


class UrlParser
{
public:
	UrlParser(const Buffer& url);
	~UrlParser(){}
	int Parser();
	Buffer operator[](const Buffer& name)const;
	Buffer Protocol()const {
		return m_protocol;
	}
	Buffer Host()const {
		return m_host;
	}
	// 默认返回80
	int Port() const {
		return m_port;
	}
	void SetUrl(const Buffer& url);
	const Buffer Uri()const {
		return m_uri;
	}
private:
	Buffer m_url;
	Buffer m_uri;
	// 协议类型
	Buffer m_protocol;
	Buffer m_host;
	int m_port;
	std::map<Buffer, Buffer> m_values;
};

