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