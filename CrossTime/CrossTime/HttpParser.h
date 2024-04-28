#pragma once
// **httpģ��

#include "http_parser.h"
#include "Public.h"
#include <map>

class CHttpParser
{
private:
	http_parser m_parser;
	http_parser_settings m_settings;
	std::map<Buffer, Buffer> m_HeaderValues; // ÿ���ֶζ�Ӧ��ֵ
	Buffer m_status;
	Buffer m_url;
	Buffer m_body;
	bool m_complete;
	Buffer m_lastField;
public:

	CHttpParser();
	~CHttpParser();
	CHttpParser(const CHttpParser& http);
	CHttpParser& operator=(const CHttpParser& http);
public:
	// http���ݽ���
	size_t Parser(const Buffer& data);
	// GET POST ...  �ο�http_parser.h HTTP_METHOD_MAP��
	unsigned Method() const {
		return m_parser.method;
	}
	// http�ײ������ļ�ֵ��
	const std::map<Buffer, Buffer>& Headers() const {
		return m_HeaderValues;
	}
	const Buffer& Status() const {
		return m_status;
	}
	// URL����ı������ü�ֵ�Ա�ʾ���ǵ�ַ?�������Щ������ֵ
	const Buffer& Url() const {
		return m_url;
	}
	// ����
	const Buffer& Body() const {
		return m_body;
	}
	// ������
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

	// ͨ����̬�Ĺ��캯�������ŵ����ǵĳ�Ա�����ϣ����ǳ�ʼ����ʱ���Ǹ�����ָ��ģ�
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
	// Ĭ�Ϸ���80
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
	// Э������
	Buffer m_protocol;
	Buffer m_host;
	int m_port;
	std::map<Buffer, Buffer> m_values;
};

