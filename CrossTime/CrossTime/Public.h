#pragma once
// **公共类模块

#include <string.h>
#include <string>
#include <signal.h>
#include <sys/stat.h>   // umask函数头文件
#include <arpa/inet.h>  // inet_addr头文件
#include <netinet/in.h> // sockaddr_in头文件
#include "logger/Logging.h"
#include "logger/LogFile.h" // muduo日志库

#define INVALID_SOCKET -1

// Linux
typedef unsigned char       BYTE;
typedef int                 SOCKET;
typedef unsigned short      WORD;
typedef unsigned int        DWORD;
typedef int                 LONG;

class CCrossTimeServer;   // 主业务类

typedef struct tagPOINT
{
	LONG  x;
	LONG  y;
} POINT;

// 设备信息
struct equipInfo
{
	equipInfo()
		: width(-1)
		, height(-1)
		, eStatus(-1)
		, thread(-1)
	{}
	std::string equipName;   // 设备名称
	int         width;       // 设备屏幕的宽度
	int         height;      // 设备屏幕的高度
	int         eStatus;     // 设备状态  1 空闲 2 被控制
	long long   thread;      // 对应开启的线程的id -1表示没有
	// TODO:...
};

// 客户端信息（包括被控端以及前端）
struct clientInfo
{
	clientInfo()
		: isEquip(false)
	{}
	sockaddr_in clientAddr;  // 客户端地址信息
	bool        isEquip;     // 是否为设备的信息 true表示是
	equipInfo   eInfo;       // 设备信息
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
	WORD  EType;       // 事件的类型   1表示鼠标事件 2表示键盘事件
	POINT MouseXY;     // 鼠标坐标
	WORD  MKOperation; // 鼠标键盘事件类型  0b00010000表示按下 0b00100000表示抬起 0b00000001表示右键 0b00000010表示中键 0b00000100表示左键
	DWORD MKKeyValue;  // 按下的键值 
}MKEVENT, * PMKEVENT;



// 设备监控处理线程的信息
struct screenArgs
{
	screenArgs(CCrossTimeServer* p, SOCKET sock, const std::string& eName)
		: thiz(p), beSocket(sock)
		, eName(eName)
	{}
	// 拷贝构造
	screenArgs(const screenArgs& args)
		: thiz(args.thiz)
		, beSocket(args.beSocket)
		, eName(args.eName)
	{}
	// 等于号重载
	screenArgs& operator=(const screenArgs& args) {
		if (this != &args) {
			thiz = args.thiz;
			beSocket = args.beSocket;
			eName = args.eName;
		}
		return *this;
	}
	CCrossTimeServer* thiz;             // 指针
	SOCKET            beSocket;         // 被控端的套接字
	std::string       eName;            // 设备名
};

// 缓冲区类
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
	// 移动构造，支持临时变量构造
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
	// 设置缓冲区数据
	void SetData(const void* data, size_t size) {
		clear();
		resize(size);
		if (data != NULL) {
			memcpy((char*)c_str(), data, size);
		}
	}
	// 转换为数字（长整型）
	long long asLONGLONG() {
		return atoll(c_str());
	}
	// 转换为数字（双精度）
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
};