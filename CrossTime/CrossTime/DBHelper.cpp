#include "DBHelper.h"
#include "CrossTimeServer.h"

DBHelper::DBHelper()
{
	if (NULL == mysql_init(&m_mysql)) {
		// mysql��ʼ��ʧ��
		ShowError();
	}
}

DBHelper::~DBHelper()
{
	mysql_close(&m_mysql);
}

bool DBHelper::Connect(const std::string& user, const std::string& password, const std::string& dbName, const std::string& host, unsigned int port)
{
	if (NULL == mysql_real_connect(&m_mysql, host.c_str(), user.c_str(), password.c_str(), dbName.c_str(), port, NULL, 0)) {
		// ���ݿ�����ʧ��
		ShowError();
		return false;
	}
	m_user = user;
	m_password = password;
	m_dbName = dbName;
	return true;
}

bool DBHelper::ChangeUser(const char* user, const char* password, const char* dbName)
{
	if (user == NULL && dbName == NULL)return false;
	// ��ʱ��ǰ�û������ݿ⣬��ֹ������Ҫ�ı���û��������ݿⲻ�ɹ����������г�Ա������������
	std::string _user(m_user), _password(m_password), _dbName(m_dbName);
	bool change = false; // �û������ݿ��Ƿ�ı� falseû�иı� true�����ı�
	if (user && std::string(user) != _user) {
		// ��Ҫ�����û�
		if (password) {
			_user = user;
			_password = password;
			change = true;
		}
	}
	if (dbName && std::string(dbName) != _dbName) {
		_dbName = dbName;
		change = true;
	}
	if (change) {
		if (0 == mysql_change_user(&m_mysql, _user.c_str(), _password.c_str(), _dbName.c_str())) {
			m_user = _user;
			m_password = _password;
			m_dbName = _dbName;
		}
		else {
			// �ı��û��������ݿ�ʧ��
			ShowError();
			return false;
		}
	}
	return true;
}

bool DBHelper::Execute(const std::string& sql)
{
	if (sql.size() <= 0)return false;
	if (mysql_real_query(&m_mysql, sql.c_str(), strlen(sql.c_str()))) {
		// ִ��ʧ��
		if (2013 == ShowError()) {
			// ���ݿ�Ͽ����ӣ������������ݿ�
			if (NULL == mysql_real_connect(&m_mysql, "localhost", m_user.c_str(), m_password.c_str(), m_dbName.c_str(), 3306, NULL, 0)) {
				// ���ݿ�����ʧ��
				ShowError();
				return false;
			}
		}
		return false;
	}
	return true;
}

const ResultRecord* const DBHelper::GetResult()
{
	// ��ȡ�����
	MYSQL_RES* res = mysql_store_result(&m_mysql);
	if (res == NULL) {
		ShowError();
		return NULL;
	}
	ResultRecord* resRecord = new ResultRecord();
	if (resRecord == NULL)return NULL;
	resRecord->LoadResult(res);
	mysql_free_result(res);
	return resRecord;
}

void DBHelper::FreeResult(const ResultRecord* const result)
{
	if (result == NULL)return;
	delete result;
}

unsigned int DBHelper::ShowError()
{
	unsigned int Errno = mysql_errno(&m_mysql);
	if (Errno) {
		LOG_ERROR << "(" << Errno << ")<" << mysql_error(&m_mysql) << ">";
		CCrossTimeServer::m_logFile->flush();
	}
	return Errno;
}