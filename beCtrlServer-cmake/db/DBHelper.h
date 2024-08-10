#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <mysql/mysql.h>
#include "Public.h"

// �ֶ���Ϣ
class field
{
public:
	field() :field_name() { field_type = -1; }
	field(const std::string& name, int type)
		: field_name(name)
		, field_type(type)
	{}
	field(const field& f)
		: field_name(f.field_name)
		, field_type(f.field_type)
	{}
	field& operator=(const field& f) {
		if (this != &f) {
			field_name = f.field_name;
			field_type = f.field_type;
		}
		return *this;
	}
	~field() {}
public:
	std::string field_name;  // �ֶ�����
	int field_type;  // �ֶ����ͣ��ο�enum_field_types
	/*enum enum_field_types {
		MYSQL_TYPE_DECIMAL,
		MYSQL_TYPE_TINY,
		MYSQL_TYPE_SHORT,
		MYSQL_TYPE_LONG,
		MYSQL_TYPE_FLOAT,
		MYSQL_TYPE_DOUBLE,
		MYSQL_TYPE_NULL,
		MYSQL_TYPE_TIMESTAMP,
		MYSQL_TYPE_LONGLONG,
		MYSQL_TYPE_INT24,
		MYSQL_TYPE_DATE,
		MYSQL_TYPE_TIME,
		MYSQL_TYPE_DATETIME,
		MYSQL_TYPE_YEAR,
		MYSQL_TYPE_NEWDATE, //< Internal to MySQL. Not used in protocol
		MYSQL_TYPE_VARCHAR,
		MYSQL_TYPE_BIT,
		MYSQL_TYPE_TIMESTAMP2,
		MYSQL_TYPE_DATETIME2,   //< Internal to MySQL. Not used in protocol
		MYSQL_TYPE_TIME2,       //< Internal to MySQL. Not used in protocol
		MYSQL_TYPE_TYPED_ARRAY, //< Used for replication only
		MYSQL_TYPE_INVALID = 243,
		MYSQL_TYPE_BOOL = 244, //< Currently just a placeholder
		MYSQL_TYPE_JSON = 245,
		MYSQL_TYPE_NEWDECIMAL = 246,
		MYSQL_TYPE_ENUM = 247,
		MYSQL_TYPE_SET = 248,
		MYSQL_TYPE_TINY_BLOB = 249,
		MYSQL_TYPE_MEDIUM_BLOB = 250,
		MYSQL_TYPE_LONG_BLOB = 251,
		MYSQL_TYPE_BLOB = 252,
		MYSQL_TYPE_VAR_STRING = 253,
		MYSQL_TYPE_STRING = 254,
		MYSQL_TYPE_GEOMETRY = 255
	}; */
};

// ����е��ֶ���Ϣ
class FieldInfos : public std::vector<field>
{
public:
	FieldInfos()
		:std::vector<field>() {}
	// ��ʼ������Ԥ��������С
	FieldInfos(size_t reserved) { reserve(reserved); }
};

// �����ʾһ��
class Record :public std::vector<Buffer>
{
public:
	Record()
		:std::vector<Buffer>() {}
	// ��ʼ������Ԥ����С
	Record(size_t reserved) { reserve(reserved); }
};

// ����еļ�¼��Ϣ
// typedef std::vector<Record> Records;

class Records :public std::vector<Record>
{
public:
	Records()
		: fieldInfos()
	{}
	Records(const Record& r, const FieldInfos& f) {
		resize(1);
		at(0).resize(r.size());
		for (unsigned int j = 0; j < r.size(); j++) {
			at(0)[j] = r[j];
		}

		fieldInfos = f;
	}
	// ���캯��
	Records(const Records& rs) {
		fieldInfos = rs.fieldInfos;
		resize(rs.size());
		for (unsigned long long i = 0; i < size(); i++) {
			at(i).resize(rs[i].size());
			for (unsigned int j = 0; j < rs[i].size(); j++) {
				at(i)[j] = rs[i][j];
			}
		}
	}
	// ���ں�����
	Records& operator=(const Records& rs) {
		if (this != &rs) {
			fieldInfos = rs.fieldInfos;
			resize(rs.size());
			for (unsigned long long i = 0; i < size(); i++) {
				at(i).resize(rs[i].size());
				for (unsigned int j = 0; j < rs[i].size(); j++) {
					at(i)[j] = rs[i][j];
				}
			}
		}
		return *this;
	}
	// ��ȡ�ֶ���Ϣ
	FieldInfos& getFieldInfos() {
		return fieldInfos;
	}
	// ��մ洢������
	void Clear() {
		fieldInfos.clear();
		for (size_t i = 0; i < size(); i++) {
			at(i).clear();
			// records[i].clear();
		}
		clear();
	}

	/// �������������Ǽ̳й�ϵ����������д���븸��ͬ���ĺ����������еĺ����ͻᱻ���ǵ�����Ҫ��ʾ����һ��
	reference
		operator[](size_type __n) _GLIBCXX_NOEXCEPT
	{
		__glibcxx_requires_subscript(__n);
		return *(this->_M_impl._M_start + __n);
	}

	const_reference
		operator[](size_type __n) const _GLIBCXX_NOEXCEPT
	{
		__glibcxx_requires_subscript(__n);
		return *(this->_M_impl._M_start + __n);
	}

	const Buffer operator[](const std::string& index) const {
		if (size() != 1 || fieldInfos.size() <= 0) {
			return std::string();
		}
		int rindex = -1;
		for (size_t i = 0; i < fieldInfos.size(); i++) {
			if (fieldInfos[i].field_name == index) {
				rindex = (int)i;
				break;
			}
		}
		if (rindex == -1)return std::string();
		return at(0)[rindex];
	}

private:
	mutable FieldInfos fieldInfos; // �ֶ���Ϣ
};

// �����¼
class ResultRecord
{
public:
	ResultRecord()
		: records()
	{}
	// �ӽ�����м�������
	bool LoadResult(MYSQL_RES* result) {
		if (result == NULL)return false;
		// row�� column��
		unsigned int column = mysql_num_fields(result);
		if (column <= 0)return false;
		// ��ȡ�ֶ���Ϣ
		MYSQL_FIELD* pField = mysql_fetch_fields(result);
		FieldInfos& fi = records.getFieldInfos();
		fi.resize(column);
		for (unsigned int i = 0; i < column; i++) {
			fi[i].field_name = pField[i].name;
			fi[i].field_type = (int)pField[i].type;
		}
		// ��ȡ��¼
		unsigned long long row = mysql_num_rows(result);
		if (row <= 0) {
			Clear();
			return false;
		}
		records.resize(row);
		for (unsigned long long i = 0; i < row; i++) {
			MYSQL_ROW mRow = mysql_fetch_row(result); // ��ȡÿһ�м�¼
			if (mRow == NULL) {
				break;
			}
			records[i].resize(column);
			for (unsigned int j = 0; j < column; j++) {
				if (fi[j].field_type == MYSQL_TYPE_BIT) {// �����ʾbit���͵ı��� ���������ͽ������⴦��
					
					records[i][j] = ((*mRow[j]) & 0x01) == 0 ? "0" : "1";
				}
				else {
					records[i][j] = mRow[j];
				}
			}
		}
		return true;
	}
	// ��մ洢������
	void Clear() {
		records.Clear();
	}
	~ResultRecord() {
		Clear();
	}
public: // ��������
	// ��ȡ�ֶ���Ϣ
	const FieldInfos& GetFieldInfos() const {
		return records.getFieldInfos();
	}
	// ��ȡ��¼��Ϣ
	const Records& GetRecords() const {
		return records;
	}
	// ��ȡ��¼��������
	size_t GetRecordsSize() const {
		return records.size();
	}
	const Records operator[](size_t index) const {
		if (index >= records.size()) {
			return Records();
		}
		return Records(records.at(index), records.getFieldInfos());
		// return records.at(index);
	}
private:
	mutable Records records; // ��¼��Ϣ
};

// ���ݿ�����
class DBHelper
{
public:
	DBHelper();
	~DBHelper();
	// �������ݿ�
	bool Connect(
		const std::string& user, const std::string& password,
		const std::string& dbName, const std::string& host = "localhost",
		unsigned int port = 3306
	);
	// �ı䵱ǰ���ӵ��û������ݿ⣬�������ΪNULL��ʹ���ϴ�����ʱ��ʹ�õ��û��������ݿ�
	bool ChangeUser(const char* user, const char* password, const char* dbName);
	// ִ��SQL���
	bool Execute(const std::string& sql);
	// ��ȡ��ѯ�����������ؽ����ΪNULL����������FreeResult�ͷŽ����
	const ResultRecord* const GetResult();
	// �ͷŽ����¼
	void FreeResult(const ResultRecord* const result);
private:
	// ��ʾ���ݿ������Ϣ
	unsigned int ShowError();

private:
	MYSQL m_mysql;
	std::string m_user; // �������ݿ��û���
	std::string m_password; // ����
	std::string m_dbName;  // ���ݿ���
};

