#pragma once
// **������ģ��

#include <string.h>
#include <string>
#include <arpa/inet.h>  // inet_addrͷ�ļ�
#include <netinet/in.h> // sockaddr_inͷ�ļ�
#include "logger/Logging.h"
#include "logger/LogFile.h" // muduo��־��

#define INVALID_SOCKET -1  // ��Ч���׽���ֵ

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
#if 0
struct equipInfo
{
	equipInfo()
		: width(-1)
		, height(-1)
		, eStatus(-1)
	{}
	std::string equipName;   // �豸����
	std::string eKey;        // �豸��Կ
	int         width;       // �豸��Ļ�Ŀ��
	int         height;      // �豸��Ļ�ĸ߶�
	int         eStatus;     // �豸״̬  1 ���� 2 ������
	// TODO:...
};
#else
// �豸��Ϣ
struct equipInfo
{
	equipInfo()
		: width(-1)
		, height(-1)
		, eStatus(-1)
		, thread(-1)
		, type(-1)
	{}
	std::string equipName;   // �豸����
	std::string eKey;        // �豸��Կ
	int         width;       // �豸��Ļ�Ŀ��
	int         height;      // �豸��Ļ�ĸ߶�
	int         eStatus;     // �豸״̬  1 ���� 2 ������
	long long   thread;      // ��Ӧ�������̵߳�id -1��ʾû��
	int         type;        // �豸����  1��Windowsϵͳ  2��Ƕ��ʽ
	// TODO:...
};
#endif

// �����豸����Ϣ�����ڱ����Ƶ��û����д���Ϣ��
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
	unsigned short recvImagePort;       // ����ͼƬ�Ķ˿�
	std::string    ctrlerId;            // �����ߵ�id
	std::string    imageDir;            // ͼƬ��ŵľ���λ��
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
	SOCKET         recvImageSock;       // ����ͼƬ���׽���
	unsigned short recvImagePort;       // ����ͼƬ�Ķ˿�
	std::string    ctrlerId;            // �����ߵ�id
	std::string    imageDir;            // ͼƬ��ŵľ���λ��
	SOCKET         sendImageClientSock; // ����ͼƬ�ı��ض��׽���
	// TODO:...
} SUPEQUIPINFO;

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
#endif

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

// ͼƬ����
typedef struct ImageData
{
public:
	char                m_name[64];   // ͼƬ��
	std::string         m_data;       // ͼƬ����
private:
	mutable std::string Data;         // ������
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="ImageName">ͼƬ��</param>
	/// <param name="size">����ImageName��ռ��С</param>
	ImageData(const char* ImageName = NULL, size_t size = 0)
		: m_name("")
	{
		if (size >= 64)
			size = 64;
		if (ImageName)
			memcpy(m_name, ImageName, size);
	}
	/// <summary>
	/// ���ݽ���
	/// </summary>
	/// <param name="pData">��������</param>
	/// <param name="size">��С</param>
	/// <returns>����ֵ��ʾ�����Ƿ�ɹ��� true��ʾ�ɹ�  false��ʾʧ��</returns>
	bool parse(const char* pData, size_t size) {
		if (size < sizeof(m_name))
			return false;
		m_data.resize(size - sizeof(m_name));
		memcpy(m_name, pData, sizeof(m_name));
		memcpy((char*)m_data.c_str(), pData + sizeof(m_name), size - sizeof(m_name));
		return true;
	}
	/// <summary>
	/// ������ת��Ϊ�ֽ���
	/// </summary>
	/// <returns>����ֵ��ʾת���������</returns>
	const char* toBytes() const {
		Data.resize(64 + m_data.size());
		memcpy((char*)Data.c_str(), m_name, sizeof(m_name));
		memcpy((char*)Data.c_str() + sizeof(m_name), m_data.c_str(), m_data.size());
		return Data.c_str();
	}
	/// <summary>
	/// ���ݴ�С
	/// </summary>
	/// <returns>����ֵ��ʾת���ֽ������ݵĴ�С</returns>
	size_t Size() const {
		return 64 + m_data.size();
	}
}IMAGEDATA;


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
	long long asLONGLONG() const {
		return atoll(c_str());
	}
	// ת��Ϊ���֣�˫���ȣ�
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