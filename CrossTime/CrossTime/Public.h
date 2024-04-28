#pragma once
// **������ģ��

#include <string.h>
#include <string>
#include <signal.h>
#include <sys/stat.h>   // umask����ͷ�ļ�
#include <arpa/inet.h>  // inet_addrͷ�ļ�
#include <netinet/in.h> // sockaddr_inͷ�ļ�
#include "logger/Logging.h"
#include "logger/LogFile.h" // muduo��־��

#define INVALID_SOCKET -1

// Linux
typedef unsigned char       BYTE;
typedef int                 SOCKET;
typedef unsigned short      WORD;
typedef unsigned int        DWORD;
typedef int                 LONG;

class CCrossTimeServer;   // ��ҵ����

typedef struct tagPOINT
{
	LONG  x;
	LONG  y;
} POINT;

// �豸��Ϣ
struct equipInfo
{
	equipInfo()
		: width(-1)
		, height(-1)
		, eStatus(-1)
		, thread(-1)
	{}
	std::string equipName;   // �豸����
	int         width;       // �豸��Ļ�Ŀ��
	int         height;      // �豸��Ļ�ĸ߶�
	int         eStatus;     // �豸״̬  1 ���� 2 ������
	long long   thread;      // ��Ӧ�������̵߳�id -1��ʾû��
	// TODO:...
};

// �ͻ�����Ϣ���������ض��Լ�ǰ�ˣ�
struct clientInfo
{
	clientInfo()
		: isEquip(false)
	{}
	sockaddr_in clientAddr;  // �ͻ��˵�ַ��Ϣ
	bool        isEquip;     // �Ƿ�Ϊ�豸����Ϣ true��ʾ��
	equipInfo   eInfo;       // �豸��Ϣ
	// TODO:...
};

typedef struct MKEvent {
	MKEvent()
		: EType(0), MKOperation(0)
		, MKKeyValue(0)
	{
		MouseXY.x = 0;
		MouseXY.y = 0;
	}
	MKEvent(const MKEvent& mke)
		: EType(mke.EType), MKOperation(mke.MKOperation)
		, MKKeyValue(mke.MKKeyValue)
	{
		MouseXY.x = mke.MouseXY.x;
		MouseXY.y = mke.MouseXY.y;
	}
	MKEvent& operator=(const MKEvent& mke) {
		if (this != &mke) {
			EType = mke.EType;
			MouseXY.x = mke.MouseXY.x;
			MouseXY.y = mke.MouseXY.y;
			MKOperation = mke.MKOperation;
			MKKeyValue = mke.MKKeyValue;
		}
		return *this;
	}
	WORD  EType;       // �¼�������   1��ʾ����¼� 2��ʾ�����¼�
	POINT MouseXY;     // �������
	WORD  MKOperation; // �������¼�����  0b00010000��ʾ���� 0b00100000��ʾ̧�� 0b00000001��ʾ�Ҽ� 0b00000010��ʾ�м� 0b00000100��ʾ���
	DWORD MKKeyValue;  // ���µļ�ֵ 
}MKEVENT, * PMKEVENT;



// �豸��ش����̵߳���Ϣ
struct screenArgs
{
	screenArgs(CCrossTimeServer* p, SOCKET sock, const std::string& eName)
		: thiz(p), beSocket(sock)
		, eName(eName)
	{}
	// ��������
	screenArgs(const screenArgs& args)
		: thiz(args.thiz)
		, beSocket(args.beSocket)
		, eName(args.eName)
	{}
	// ���ں�����
	screenArgs& operator=(const screenArgs& args) {
		if (this != &args) {
			thiz = args.thiz;
			beSocket = args.beSocket;
			eName = args.eName;
		}
		return *this;
	}
	CCrossTimeServer* thiz;             // ָ��
	SOCKET            beSocket;         // ���ض˵��׽���
	std::string       eName;            // �豸��
};

// ��������
class Buffer
	:public std::string
{
public:
	Buffer(size_t size = 0)
		:std::string(size, '\0')
	{}
	Buffer(void* data, size_t size)
		:std::string(size, '\0')
	{
		if (data != NULL) {
			memcpy((void*)c_str(), data, size);
		}
	}
	Buffer(const char* str, size_t length) :std::string() {
		if (str != NULL) {
			resize(length);
			memcpy((char*)c_str(), str, length);
		}
	}
	Buffer(const char* str)
		:std::string(str)
	{}
	Buffer(const Buffer& buf)
		:std::string(buf.c_str())
	{}
	Buffer(const std::string& str) :std::string(str) {}
	// �ƶ����죬֧����ʱ��������
	Buffer(std::string&& str) :std::string(str) {}
	/*virtual ~Buffer() {
		clear();
		std::string::~basic_string();
	}*/
	Buffer(const char* begin, const char* end) :std::string() {
		if ((begin != NULL) && (end != NULL)) {
			int len = (int)(end - begin);
			if (len > 0) {
				resize(len);
				memcpy((char*)c_str(), begin, len);
			}
		}
	}
	// ���û���������
	void SetData(const void* data, size_t size) {
		clear();
		resize(size);
		if (data != NULL) {
			memcpy((char*)c_str(), data, size);
		}
	}
	// ת��Ϊ���֣������ͣ�
	long long asLONGLONG() {
		return atoll(c_str());
	}
	// ת��Ϊ���֣�˫���ȣ�
	double asDOUBLE() {
		return atof(c_str());
	}
	Buffer& operator=(const Buffer& buf) {
		if (this != &buf) {
			std::string::operator=(buf.c_str());
		}
		return *this;
	}
	Buffer& operator=(const char* str) {
		if (str == NULL) {
			std::string::operator=("");
		}
		else
			std::string::operator=(str);
		return *this;
	}
	Buffer& operator=(const std::string& str) {
		std::string::operator=(str);
		return *this;
	}
	explicit operator char* () const {
		return (char*)c_str();
	}
	operator const char* () const {
		return c_str();
	}
	explicit operator BYTE* () {
		return (BYTE*)c_str();
	}
	operator const BYTE* ()const {
		return (const BYTE*)c_str();
	}
};


class CTools
{
public:
	// �����ػ�����
	static int SwitchDeamon()
	{
		pid_t ret = fork();
		if (ret == -1)return -1;
		if (ret > 0) {
			// ������
			exit(0);
		}
		// �ӽ���
		ret = setsid();
		if (ret == -1)return -2;
		ret = fork();
		if (ret == -1)return -3;
		if (ret > 0) {
			// �ӽ���
			exit(0);
		}

		// �����
		for (int i = 0; i < 3; i++) {
			close(i);
		}
		umask(0);
		signal(SIGCHLD, SIG_IGN);
		return 0;
	}
};