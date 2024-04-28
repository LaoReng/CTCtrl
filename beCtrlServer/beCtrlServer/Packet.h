#pragma once
#include <string>
#include <string.h>


// 图片数据包
class CPacket
{
public:
	// 默认构造函数
	CPacket()
		: m_head(0XFEFF), m_cmd(0)
		, m_num(0), m_length(0)
		, m_sum(0)
	{}
	// 析构函数
	~CPacket() {
		{
			std::string temp;
			m_data.swap(temp);
		}
		{
			std::string temp;
			outData.swap(temp);
		}
		m_data.clear();
		outData.clear();
	}
	// 赋值构造
	CPacket(const CPacket& pack)
		: m_head(pack.m_head), m_cmd(pack.m_cmd)
		, m_num(pack.m_num), m_length(pack.m_length)
		, m_sum(pack.m_sum)
	{
		m_data.resize(pack.m_data.size());
		memcpy((char*)m_data.c_str(), pack.m_data.c_str(), m_data.size());
		outData.resize(pack.outData.size());
		memcpy((char*)outData.c_str(), pack.outData.c_str(), outData.size());
	}
	// 等于号重载
	CPacket& operator=(const CPacket& pack) {
		if (this != &pack) {
			m_head = pack.m_head;
			m_cmd = pack.m_cmd;
			m_num = pack.m_num;
			m_length = pack.m_length;
			m_data.resize(pack.m_data.size());
			memcpy((char*)m_data.c_str(), pack.m_data.c_str(), m_data.size());
			m_sum = pack.m_sum;
			outData.resize(pack.outData.size());
			memcpy((char*)outData.c_str(), pack.outData.c_str(), outData.size());
		}
		return *this;
	}
	// 封包
	CPacket(unsigned short nCmd, unsigned int nNum, const char* pData, size_t nDataSize)
		: m_head(0xFEFF), m_cmd(0), m_num(0)
		, m_length(2), m_sum(0)
	{
		if ((nNum <= 0) || (pData == NULL) || (nDataSize <= 0)) {
			return;
		}
		m_head = 0XFEFF;
		m_cmd = nCmd;
		m_num = nNum;
		m_length = nDataSize + sizeof(m_sum);
		m_data.resize(nDataSize);
		memcpy((char*)m_data.c_str(), pData, nDataSize);
		unsigned short sum = 0;
		for (size_t i = 0; i < m_data.size(); i++) {
			sum += (unsigned short)m_data[i];
		}
		m_sum = sum;
	}
	// 解包
	CPacket(const char* pData, long long& nlen) {
		long long i = 0;
		// 找到包头
		for (; i < nlen; i++) {
			if (0XFEFF == *(unsigned short*)(pData + i)) {
				m_head = *(unsigned short*)(pData + i);
				i += sizeof(m_head);
				break;
			}
		}
		if ((long long)(i + sizeof(m_cmd) + sizeof(m_num) + sizeof(m_length) + sizeof(m_sum)) > nlen) {
			// 数据不全
			nlen = 0;
			return;
		}
		// 数据包的命令号
		m_cmd = *(unsigned short*)(pData + i); i += sizeof(m_cmd);
		// 第几张图片
		m_num = *(unsigned int*)(pData + i); i += sizeof(m_num);
		// 包括数据和校验和的总长度
		m_length = *(unsigned int*)(pData + i); i += sizeof(m_length);
		if ((long long)(i + m_length) > nlen) {
			// 数据不全
			nlen = 0;
			return;
		}
		// 找到数据
		m_data.resize(m_length - sizeof(m_sum));
		memcpy((char*)m_data.c_str(), pData + i, m_data.size());
		i += m_length - sizeof(m_sum);
		// 找到数据校验和
		m_sum = *(unsigned short*)(pData + i); i += sizeof(m_sum);
		// 判断数据校验和是否一致
		unsigned short sum = 0;
		for (size_t i = 0; i < m_data.size(); i++) {
			sum += (unsigned short)m_data[i];
		}
		if (sum != m_sum) {
			// 数据校验和没通过
			nlen = -1 * i;
		}
		else {
			nlen = i;
		}
	}
	// 输出为字符串
	const char* outStr() const {
		outData.resize(Size());
		long long i = 0;
		char* pData = (char*)outData.c_str();
		memcpy(pData + i, &m_head, sizeof(m_head)); i += sizeof(m_head);
		memcpy(pData + i, &m_cmd, sizeof(m_cmd)); i += sizeof(m_cmd);
		memcpy(pData + i, &m_num, sizeof(m_num)); i += sizeof(m_num);
		memcpy(pData + i, &m_length, sizeof(m_length)); i += sizeof(m_length);
		memcpy(pData + i, m_data.c_str(), m_data.size()); i += m_data.size();
		memcpy(pData + i, &m_sum, sizeof(m_sum)); i += sizeof(m_sum);
		return outData.c_str();
	}
	// 获取命令号
	unsigned short GetCmd() const {
		return m_cmd;
	}
	// 获取图片数据
	const char* GetData() const {
		return m_data.c_str();
	}
	// 获取图片大小
	size_t GetDataSize() const {
		return m_data.size();
	}
	// 获取当前是第几张图片
	unsigned int GetNum() const {
		return m_num;
	}
	// 获取整个包的长度
	size_t Size() const {
		return sizeof(m_head) + sizeof(m_cmd) + sizeof(m_num) + sizeof(m_length) + m_length;
	}
private:
	unsigned short      m_head;           // 包头，固定格式FEFF
	unsigned short      m_cmd;            // 命令号 1监控屏幕   2停止屏幕监控   3鼠标键盘消息  
	unsigned int        m_num;            // 当前是第几张图片
	size_t              m_length;         // 长度，包括数据和校验和的总长度
	std::string         m_data;           // 数据
	unsigned short      m_sum;            // 校验和
	mutable std::string outData;          // 以字符串形式保存
};