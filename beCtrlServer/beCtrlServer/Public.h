#pragma once
// **公共类模块
#include "pch.h"
#include <string.h>
#include <string>

// Linux
typedef unsigned char BYTE;

// 宏定义
#define BUFFER_SIZE 20480000

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
	char        m_name[64];   // 图片名
	std::string m_data;       // 图片数据
private:
	mutable std::string Data;  // 总数据
public:
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="ImageName">图片名</param>
	/// <param name="size">参数ImageName所占大小</param>
	ImageData(const char* ImageName = NULL, size_t size = 0)
		: m_name("")
	{
		if (size >= sizeof(m_name))
			size = sizeof(m_name);
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
		Data.clear();
		Data.resize(sizeof(m_name) + m_data.size());
		memcpy((char*)Data.c_str(), m_name, sizeof(m_name));
		memcpy((char*)Data.c_str() + sizeof(m_name), m_data.c_str(), m_data.size());
		return Data.c_str();
	}
	/// <summary>
	/// 数据大小
	/// </summary>
	/// <returns>返回值表示转换字节流数据的大小</returns>
	size_t Size() const {
		return sizeof(m_name) + m_data.size();
	}
}IMAGEDATA;