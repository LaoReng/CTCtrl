#pragma once
#include <string>
#include <string.h>


// ͼƬ���ݰ�
class CPacket
{
public:
	// Ĭ�Ϲ��캯��
	CPacket()
		: m_head(0XFEFF), m_cmd(0)
		, m_num(0), m_length(0)
		, m_sum(0)
	{}
	// ��������
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
	// ��ֵ����
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
	// ���ں�����
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
	// ���
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
	// ���
	CPacket(const char* pData, long long& nlen) {
		long long i = 0;
		// �ҵ���ͷ
		for (; i < nlen; i++) {
			if (0XFEFF == *(unsigned short*)(pData + i)) {
				m_head = *(unsigned short*)(pData + i);
				i += sizeof(m_head);
				break;
			}
		}
		if ((long long)(i + sizeof(m_cmd) + sizeof(m_num) + sizeof(m_length) + sizeof(m_sum)) > nlen) {
			// ���ݲ�ȫ
			nlen = 0;
			return;
		}
		// ���ݰ��������
		m_cmd = *(unsigned short*)(pData + i); i += sizeof(m_cmd);
		// �ڼ���ͼƬ
		m_num = *(unsigned int*)(pData + i); i += sizeof(m_num);
		// �������ݺ�У��͵��ܳ���
		m_length = *(unsigned int*)(pData + i); i += sizeof(m_length);
		if ((long long)(i + m_length) > nlen) {
			// ���ݲ�ȫ
			nlen = 0;
			return;
		}
		// �ҵ�����
		m_data.resize(m_length - sizeof(m_sum));
		memcpy((char*)m_data.c_str(), pData + i, m_data.size());
		i += m_length - sizeof(m_sum);
		// �ҵ�����У���
		m_sum = *(unsigned short*)(pData + i); i += sizeof(m_sum);
		// �ж�����У����Ƿ�һ��
		unsigned short sum = 0;
		for (size_t i = 0; i < m_data.size(); i++) {
			sum += (unsigned short)m_data[i];
		}
		if (sum != m_sum) {
			// ����У���ûͨ��
			nlen = -1 * i;
		}
		else {
			nlen = i;
		}
	}
	// ���Ϊ�ַ���
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
	// ��ȡ�����
	unsigned short GetCmd() const {
		return m_cmd;
	}
	// ��ȡͼƬ����
	const char* GetData() const {
		return m_data.c_str();
	}
	// ��ȡͼƬ��С
	size_t GetDataSize() const {
		return m_data.size();
	}
	// ��ȡ��ǰ�ǵڼ���ͼƬ
	unsigned int GetNum() const {
		return m_num;
	}
	// ��ȡ�������ĳ���
	size_t Size() const {
		return sizeof(m_head) + sizeof(m_cmd) + sizeof(m_num) + sizeof(m_length) + m_length;
	}
private:
	unsigned short      m_head;           // ��ͷ���̶���ʽFEFF
	unsigned short      m_cmd;            // ����� 1�����Ļ   2ֹͣ��Ļ���   3��������Ϣ  
	unsigned int        m_num;            // ��ǰ�ǵڼ���ͼƬ
	size_t              m_length;         // ���ȣ��������ݺ�У��͵��ܳ���
	std::string         m_data;           // ����
	unsigned short      m_sum;            // У���
	mutable std::string outData;          // ���ַ�����ʽ����
};