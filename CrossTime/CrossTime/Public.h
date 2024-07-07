#pragma once
// **公共类模块

#include <string.h>
#include <string>
#include <arpa/inet.h>  // inet_addr头文件
#include <netinet/in.h> // sockaddr_in头文件
#include "logger/Logging.h"
#include "logger/LogFile.h" // muduo日志库

#define INVALID_SOCKET -1  // 无效的套接字值

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
#if 0
struct equipInfo
{
	equipInfo()
		: width(-1)
		, height(-1)
		, eStatus(-1)
	{}
	std::string equipName;   // 设备名称
	std::string eKey;        // 设备密钥
	int         width;       // 设备屏幕的宽度
	int         height;      // 设备屏幕的高度
	int         eStatus;     // 设备状态  1 空闲 2 被控制
	// TODO:...
};
#else
// 设备信息
struct equipInfo
{
	equipInfo()
		: width(-1)
		, height(-1)
		, eStatus(-1)
		, thread(-1)
		, type(-1)
	{}
	std::string equipName;   // 设备名称
	std::string eKey;        // 设备密钥
	int         width;       // 设备屏幕的宽度
	int         height;      // 设备屏幕的高度
	int         eStatus;     // 设备状态  1 空闲 2 被控制
	long long   thread;      // 对应开启的线程的id -1表示没有
	int         type;        // 设备类型  1：Windows系统  2：嵌入式
	// TODO:...
};
#endif

// 被控设备的信息（正在被控制的用户才有此信息）
#if 0
typedef struct SupEquipInfo
{
	SupEquipInfo()
		: recvImagePort(0)
	{}
	SupEquipInfo(const SupEquipInfo& se)
		: recvImagePort(se.recvImagePort)
		, ctrlerId(se.ctrlerId.c_str())
		, imageDir(se.imageDir.c_str())
	{}
	SupEquipInfo& operator=(const SupEquipInfo& se) {
		if (this != &se) {
			recvImagePort = se.recvImagePort;
			ctrlerId = se.ctrlerId.c_str();
			imageDir = se.imageDir.c_str();
		}
		return *this;
	}
	~SupEquipInfo() {}
	unsigned short recvImagePort;       // 接收图片的端口
	std::string    ctrlerId;            // 控制者的id
	std::string    imageDir;            // 图片存放的绝对位置
	// TODO:...
} SUPEQUIPINFO;
#else
typedef struct SupEquipInfo
{
	SupEquipInfo()
		: recvImageSock(INVALID_SOCKET)
		, recvImagePort(0)
		, sendImageClientSock(INVALID_SOCKET)
	{}
	SupEquipInfo(const SupEquipInfo& se)
		: recvImageSock(se.recvImageSock)
		, recvImagePort(se.recvImagePort)
		, ctrlerId(se.ctrlerId.c_str())
		, imageDir(se.imageDir.c_str())
		, sendImageClientSock(se.sendImageClientSock)
	{}
	SupEquipInfo& operator=(const SupEquipInfo& se) {
		if (this != &se) {
			recvImageSock = se.recvImageSock;
			recvImagePort = se.recvImagePort;
			ctrlerId = se.ctrlerId.c_str();
			imageDir = se.imageDir.c_str();
			sendImageClientSock = se.sendImageClientSock;
		}
		return *this;
	}
	~SupEquipInfo() {}
	SOCKET         recvImageSock;       // 接收图片的套接字
	unsigned short recvImagePort;       // 接收图片的端口
	std::string    ctrlerId;            // 控制者的id
	std::string    imageDir;            // 图片存放的绝对位置
	SOCKET         sendImageClientSock; // 发送图片的被控端套接字
	// TODO:...
} SUPEQUIPINFO;

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
#endif

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

// 图片数据
typedef struct ImageData
{
public:
	char                m_name[64];   // 图片名
	std::string         m_data;       // 图片数据
private:
	mutable std::string Data;         // 总数据
public:
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="ImageName">图片名</param>
	/// <param name="size">参数ImageName所占大小</param>
	ImageData(const char* ImageName = NULL, size_t size = 0)
		: m_name("")
	{
		if (size >= 64)
			size = 64;
		if (ImageName)
			memcpy(m_name, ImageName, size);
	}
	/// <summary>
	/// 数据解析
	/// </summary>
	/// <param name="pData">数据内容</param>
	/// <param name="size">大小</param>
	/// <returns>返回值表示解析是否成功， true表示成功  false表示失败</returns>
	bool parse(const char* pData, size_t size) {
		if (size < sizeof(m_name))
			return false;
		m_data.resize(size - sizeof(m_name));
		memcpy(m_name, pData, sizeof(m_name));
		memcpy((char*)m_data.c_str(), pData + sizeof(m_name), size - sizeof(m_name));
		return true;
	}
	/// <summary>
	/// 将数据转换为字节流
	/// </summary>
	/// <returns>返回值表示转换后的数据</returns>
	const char* toBytes() const {
		Data.resize(64 + m_data.size());
		memcpy((char*)Data.c_str(), m_name, sizeof(m_name));
		memcpy((char*)Data.c_str() + sizeof(m_name), m_data.c_str(), m_data.size());
		return Data.c_str();
	}
	/// <summary>
	/// 数据大小
	/// </summary>
	/// <returns>返回值表示转换字节流数据的大小</returns>
	size_t Size() const {
		return 64 + m_data.size();
	}
}IMAGEDATA;


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
	long long asLONGLONG() const {
		return atoll(c_str());
	}
	// 转换为数字（双精度）
	double asDOUBLE() const {
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