#pragma once
// **������ģ��
#include "pch.h"
#include <string.h>
#include <string>

// Linux
typedef unsigned char BYTE;

// �궨��
#define BUFFER_SIZE 20480000

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
	char        m_name[64];   // ͼƬ��
	std::string m_data;       // ͼƬ����
private:
	mutable std::string Data;  // ������
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="ImageName">ͼƬ��</param>
	/// <param name="size">����ImageName��ռ��С</param>
	ImageData(const char* ImageName = NULL, size_t size = 0)
		: m_name("")
	{
		if (size >= sizeof(m_name))
			size = sizeof(m_name);
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
		Data.clear();
		Data.resize(sizeof(m_name) + m_data.size());
		memcpy((char*)Data.c_str(), m_name, sizeof(m_name));
		memcpy((char*)Data.c_str() + sizeof(m_name), m_data.c_str(), m_data.size());
		return Data.c_str();
	}
	/// <summary>
	/// ���ݴ�С
	/// </summary>
	/// <returns>����ֵ��ʾת���ֽ������ݵĴ�С</returns>
	size_t Size() const {
		return sizeof(m_name) + m_data.size();
	}
}IMAGEDATA;