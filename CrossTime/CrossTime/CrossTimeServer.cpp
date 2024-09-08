#include "CrossTimeServer.h"
#include "Packet.h"
#include "DBHelper.h"

std::unique_ptr<LogFile>   CCrossTimeServer::m_logFile;  // 日志库初始化

CCrossTimeServer::CCrossTimeServer(const std::string& ip, unsigned short port)
	: m_serverSocket(INVALID_SOCKET)
	, m_maxFd(INVALID_SOCKET)
	, m_bServerInvalid(true)
	// , m_ThreadPool(3)
{
	// 日志库初始化
	initLogger();
	// web服务初始化
	memset(&m_serverAddr, 0, sizeof(m_serverAddr));
	FD_ZERO(&m_rdset); // 使用给的宏清空
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(port);
	m_serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	if (SelectInit() == false) {
		LOG_ERROR << "服务器初始化失败";
		m_logFile->flush();
		exit(0);
	}
	// 启动线程池
	/*if (m_ThreadPool.Invoke() == false) {
		LOG_ERROR << "线程池启动失败";
		m_logFile->flush();
		exit(0);
	}*/
	// 函数映射表注册
	initFuncMapping();
	// 数据库初始化
	if (false == initDatabase()) {
		LOG_ERROR << "数据库初始化失败";
		m_logFile->flush();
		exit(0);
	}
}

CCrossTimeServer::~CCrossTimeServer()
{
	m_bServerInvalid = false;
	Clear();
}

bool CCrossTimeServer::SelectInit()
{
	if (m_serverSocket != INVALID_SOCKET)
		return false;

	// 创建套接字
	m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_serverSocket < 0) {
		LOG_ERROR << "服务器套接字创建失败";
		m_logFile->flush();
		m_serverSocket = INVALID_SOCKET;
		return false;
	}
	// 设置允许端口复用
	if (!CTools::SetPortReuse(m_serverSocket)) {
		LOG_ERROR << "重用端口设备失败";
		m_logFile->flush();
		Clear();
		return false;
	}
	// 绑定端口
	if (bind(m_serverSocket, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr)) < 0) {
		LOG_ERROR << "服务器端口绑定失败";
		m_logFile->flush();
		Clear();
		return false;
	}
	// 监听套接字，使套接字fd变为lfd
	if (listen(m_serverSocket, 5) < 0) {
		LOG_ERROR << "服务器套接字监听失败";
		m_logFile->flush();
		Clear();
		return false;
	}
	m_maxFd = m_serverSocket;
	m_fds.push_back(m_serverSocket); // 服务器套接字单独监听
	return true;
}

bool CCrossTimeServer::Start()
{
	LOG_INFO << "selectserver start";
	m_logFile->flush();
	while (m_bServerInvalid) {
		// 设置rdset
		if (false == SetRdSet()) {
			break;
		}
		m_maxFd = maxFind();
		int ret = select(m_maxFd + 1, &m_rdset, NULL, NULL, NULL); // timeout为NULL表示无限等待
		if (ret < 0) {
			break;
		}
		if (FD_ISSET(m_serverSocket, &m_rdset) == true) {
			// 有连接请求
			if (false == Accept()) {
				// 警告，客户端连接失败
				LOG_ERROR << "客户端连接失败";
				m_logFile->flush();
			}
			else {
				LOG_INFO << "客户端连接成功";
				m_logFile->flush();
			}
		}
		auto it = ++m_fds.begin();
		while (it != m_fds.end()) {
			if (FD_ISSET(*it, &m_rdset)) {
				int ret = Dispose(*it);
				switch (ret) {
				case -1: // 表示客户端主动断开连接
				{
					LOG_INFO << "客户端断开连接";
					m_logFile->flush();
					FD_CLR(*it, &m_rdset);
					close(*it);
					auto ms = m_mapSockAddr.find((*it));

					if (ms->second.isEquip == true) {
						// 这是一个设备，可能还在监控
						if (ms->second.eInfo.thread != -1) {

						}
					}

					if (ms == m_mapSockAddr.end()) {
						// 没用这个东西
						LOG_ERROR << "删除失败，没用这个东西";
						m_logFile->flush();
					}
					else {
						LOG_INFO << "已从映射表中删除该套接字";
						m_logFile->flush();
						m_mapSockAddr.erase(ms);
					}
					m_fds.erase(it++);  // 删除数组中的客户端数据
					it--;
					break;
				}
				}
			}
			it++;
		}
	}
	Clear();
	return false;
}

void CCrossTimeServer::Stop()
{
	m_bServerInvalid = false;
}

int CCrossTimeServer::Dispose(SOCKET clientSocket)
{
	// HTTP协议解析
	char buf[BUFFER_SIZE] = "";
	// char* buf = new char[BUFFER_SIZE];
	ssize_t len = read(clientSocket, buf, sizeof(buf));
	LOG_INFO << "read len=" << len << "buffer:[" << buf << "]";
	m_logFile->flush();
	if (len <= 0) {
		// 客户端主动断开连接
		LOG_INFO << "客户端主动断开连接";
		m_logFile->flush();
		return -1;
	}
	// 变成任务投递到线程池中 TODO:有问题待改进

	// ThreadWorker worker(CCrossTimeServer::MyWorkerEny, (void*)this, Buffer(buf), clientSocket);
	// m_ThreadPool.DispatchWorker(worker);

	std::string bodyData; // 应答包中的body数据部分
	int ret = HttpParser(buf, bodyData, clientSocket);
	if (ret < 0) {
		LOG_WARN << "http数据解析失败";
		m_logFile->flush();
		return -1;
	}
	Buffer response = MakeAckPacket(bodyData);
	LOG_INFO << "response body[" << bodyData << "]";
	m_logFile->flush();
	len = send(clientSocket, response.c_str(), response.size(), 0);
	if (len <= 0) {
		return -1;
	}
	return 0;
}

bool CCrossTimeServer::Accept()
{
	sockaddr_in clientAddr = {};
	socklen_t len = sizeof(clientAddr);
	SOCKET clientSock = accept(m_serverSocket, (sockaddr*)&clientAddr, &len);
	if (clientSock < 0) {

		return false;
	}
	clientInfo cliInfo = {};
	cliInfo.clientAddr = clientAddr;
	m_fds.push_back(clientSock);
	m_mapSockAddr[clientSock] = cliInfo;

	m_maxFd = m_maxFd < clientSock ? clientSock : m_maxFd;
	return true;
}

void CCrossTimeServer::Clear()
{
	// 清空所有连接上来的客户端
	if (m_fds.size() > 0) {
		for (auto it : m_fds) {
			close(it);
		}
		/*for (size_t i = 0; i < m_fds.size(); i++) {
			close(m_fds[i]);
		}*/
		m_fds.clear();
	}
	// 关闭服务器套接字
	if (m_serverSocket != INVALID_SOCKET) {
		SOCKET temp = m_serverSocket;
		m_serverSocket = INVALID_SOCKET;
		close(temp);
	}
	m_maxFd = -1;
	memset(&m_rdset, 0, sizeof(m_rdset));
	memset(&m_serverAddr, 0, sizeof(m_serverAddr));
}

bool CCrossTimeServer::SetRdSet()
{
	FD_ZERO(&m_rdset); // 使用给的宏清空
	if (m_fds.size() <= 0)return false;
	for (auto it : m_fds) {
		FD_SET(it, &m_rdset);
	}
	/*for (size_t i = 0; i < m_fds.size(); i++) {
		FD_SET(m_fds[i], &m_rdset);
	}*/
	return true;
}

int CCrossTimeServer::HttpParser(const Buffer& data, std::string& bodyData, SOCKET clientSocket)
{
	CHttpParser parser;
	int ret = -1;
	size_t size = parser.Parser(data);
	if ((size == 0) || (parser.Errno() != 0)) {
		LOG_ERROR << "size = " << size << " http errno = " << parser.Errno();
		m_logFile->flush();
		return ret;
	}
	UrlParser url("https://127.0.0.1" + parser.Url());
	ret = url.Parser();
	if (ret != 0) {
		LOG_ERROR << "ret = " << ret << " url:[" << ("https://127.0.0.1" + parser.Url()) << "]";
		m_logFile->flush();
		return ret;
	}
	switch (parser.Method())
	{
	case HTTP_OPTIONS: {
		LOG_INFO << "HTTP_OPTIONS uri:[" << url.Uri() << "]";
		m_logFile->flush();
		ret = 0;
		break;
	}
	case HTTP_GET: {
		// get处理
		Buffer uri = url.Uri();
		LOG_INFO << "HTTP_GET uri:[" << uri.c_str() << "]";
		m_logFile->flush();
		auto it = m_mGETUriDisfunc.find(uri);
		if (it != m_mGETUriDisfunc.end()) {
			ret = (this->*it->second)(url, parser, bodyData, clientSocket);
		}
		else {
			// 没有找到对应的uri
			LOG_INFO << "no find uri Dispose uri:" << uri.c_str();
			m_logFile->flush();
		}
		break;
	}
	case HTTP_POST: {
		// post 处理
		Buffer uri = url.Uri();
		LOG_INFO << "HTTP_POST uri:[" << uri.c_str() << "]";
		m_logFile->flush();
		auto it = m_mPOSTUriDisfunc.find(uri);
		if (it != m_mPOSTUriDisfunc.end()) {
			ret = (this->*it->second)(url, parser, bodyData, clientSocket);
		}
		else {
			// 没有找到对应的uri
			LOG_INFO << "no find uri Dispose uri:" << uri.c_str();
			m_logFile->flush();
		}
		break;
	}
	case HTTP_DELETE: {
		// delete 处理
		Buffer uri = url.Uri();
		LOG_INFO << "HTTP_DELETE uri:[" << uri.c_str() << "]";
		m_logFile->flush();
		auto it = m_mDELETEUriDisfunc.find(uri);
		if (it != m_mDELETEUriDisfunc.end()) {
			ret = (this->*it->second)(url, parser, bodyData, clientSocket);
		}
		else {
			// 没有找到对应的uri
			LOG_INFO << "no find uri Dispose uri:" << uri.c_str();
			m_logFile->flush();
		}
		break;
	}
	}
	return ret;
}

bool CCrossTimeServer::initDatabase()
{
	// CTCtrl666
	bool ret = m_db.Connect(DB_USER, DB_PASSWORD, "");
	if (ret == false) {
		LOG_ERROR << "数据库连接失败";
		m_logFile->flush();
		return false;
	}
	// 创建数据库
	std::string sql = "CREATE DATABASE IF NOT EXISTS `CrossTime`;";
	if (false == m_db.Execute(sql)) {
		return false;
	}

	// 创建用户表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`ctuser` (\
		`Uid` int(11) NOT NULL AUTO_INCREMENT COMMENT '用户id',\
		`Uname` varchar(40) CHARACTER SET utf8 NOT NULL COMMENT '用户名',\
		`Uavatar` varchar(40) DEFAULT NULL COMMENT '用户头像',\
		`Upassword` char(15) NOT NULL COMMENT '密码',\
		`Usex` char(4) DEFAULT '2' COMMENT '性别',\
		`Uaddr` varchar(40) CHARACTER SET utf8 DEFAULT NULL COMMENT '住址',\
		`Uphone` char(12) DEFAULT NULL COMMENT '电话',\
		`Uregister_time` datetime NOT NULL COMMENT '注册时间',\
		`Ubalance` double NOT NULL COMMENT '余额',\
		`Uforbidden` bit(1) NOT NULL COMMENT '禁用',\
		`Uexpire` bit(1) NOT NULL COMMENT '停用',\
		PRIMARY KEY(`Uid`)\
		) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8; ";
	if (false == m_db.Execute(sql)) {
		return false;
	}
	// 创建忘记密码表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`revert_password` (\
		`Uid` int(11) NOT NULL COMMENT '用户id',\
		`question1` varchar(40) CHARACTER SET utf8 NOT NULL COMMENT '问题1',\
		`answer1` varchar(40) CHARACTER SET utf8 NOT NULL COMMENT '答案1',\
		`question2` varchar(40) CHARACTER SET utf8 NOT NULL COMMENT '问题2',\
		`answer2` varchar(40) CHARACTER SET utf8 NOT NULL COMMENT '答案2',\
		PRIMARY KEY(`Uid`),\
			CONSTRAINT `ctuser` FOREIGN KEY(`Uid`) REFERENCES `ctuser` (`Uid`) ON DELETE CASCADE ON UPDATE CASCADE\
				) ENGINE=InnoDB DEFAULT CHARSET=utf8; ";
	if (false == m_db.Execute(sql)) {
		return false;
	}
	// 创建VIP表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`vip` (\
		`Uid` int(11) NOT NULL COMMENT '用户id',\
		`status` bit(1) NOT NULL COMMENT 'vip状态',\
		`grade` int(11) NOT NULL COMMENT 'vip等级',\
		`acc_amount` double NOT NULL COMMENT '累计充值金额',\
		`expire_time` datetime NOT NULL COMMENT '到期时间',\
		`ctrl_time` int(11) NOT NULL COMMENT '用户可控设备时间',\
		`ctrl_num` int(11) NOT NULL COMMENT '可控次数',\
		`surplus_num` int(11) NOT NULL COMMENT '剩余次数',\
		PRIMARY KEY(`Uid`),\
			CONSTRAINT `Uid` FOREIGN KEY(`Uid`) REFERENCES `ctuser` (`Uid`) ON DELETE CASCADE ON UPDATE CASCADE\
				) ENGINE=InnoDB DEFAULT CHARSET=utf8; ";
	if (false == m_db.Execute(sql)) {
		return false;
	}
	// 创建登录记录表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`ulogin_history` (\
		`id` int NOT NULL AUTO_INCREMENT COMMENT '记录id',\
		`Uid` int NOT NULL COMMENT '用户id',\
		`ipAddr` varchar(40) NOT NULL COMMENT 'ip地址',\
		`longitude` double NOT NULL COMMENT '经度',\
		`latitude` double NOT NULL COMMENT '纬度',\
		`address` varchar(255) NOT NULL COMMENT '登录地点',\
		`dtype` varchar(40) NOT NULL COMMENT '登录设备类型',\
		`loginTime` datetime NOT NULL COMMENT '登录时间',\
		PRIMARY KEY(`id`),\
			KEY `userid` (`Uid`),\
				CONSTRAINT `userid` FOREIGN KEY(`Uid`) REFERENCES `ctuser` (`Uid`) ON DELETE CASCADE ON UPDATE CASCADE\
					) ENGINE = InnoDB DEFAULT CHARSET = utf8; ";
	if (false == m_db.Execute(sql)) {
		return false;
	}
	// 创建设备表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`equipment` (\
		`Eid` int(11) NOT NULL AUTO_INCREMENT COMMENT '设备id',\
		`Ename` varchar(40) CHARACTER SET utf8 NOT NULL COMMENT '设备名',\
		`Uid` int(11) NOT NULL COMMENT '所属用户id',\
		`secret_key` varchar(40) NOT NULL COMMENT '控制密钥',\
		`app_time` datetime NOT NULL COMMENT '申请时间',\
		`app_status` bit(1) NOT NULL COMMENT '申请状态',\
		`longitude` double NOT NULL COMMENT '经度',\
		`latitude` double NOT NULL COMMENT '纬度',\
		PRIMARY KEY(`Eid`),\
			KEY `belone_id` (`Uid`),\
				CONSTRAINT `belone_id` FOREIGN KEY(`Uid`) REFERENCES `ctuser` (`Uid`) ON DELETE CASCADE ON UPDATE CASCADE\
					) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8; ";
	if (false == m_db.Execute(sql)) {
		return false;
	}
	// 创建管理员表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`manager` (\
		`id` int(11) NOT NULL AUTO_INCREMENT COMMENT '管理员id',\
		`name` varchar(40) CHARACTER SET utf8 NOT NULL COMMENT '管理员名',\
		`phone` char(12) NOT NULL COMMENT '电话',\
		`password` char(15) NOT NULL COMMENT '密码',\
		`register_time` datetime NOT NULL COMMENT '注册时间',\
		PRIMARY KEY(`id`)\
		) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8; ";
	if (false == m_db.Execute(sql)) {
		return false;
	}
	// 创建公告表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`notice` (\
		`id` int(11) NOT NULL AUTO_INCREMENT COMMENT '公告id',\
		`mid` int(11) DEFAULT NULL COMMENT '发布者id',\
		`title` varchar(40) CHARACTER SET utf8 NOT NULL COMMENT '标题',\
		`content` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '内容',\
		`create_time` datetime NOT NULL COMMENT '创建时间',\
		`update` datetime NOT NULL COMMENT '更新时间',\
		PRIMARY KEY(`id`),\
			KEY `mid` (`mid`),\
				CONSTRAINT `mid` FOREIGN KEY(`mid`) REFERENCES `manager` (`id`) ON DELETE CASCADE ON UPDATE CASCADE\
					) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8; ";
	if (false == m_db.Execute(sql)) {
		return false;
	}
	// 创建用户可控设备表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`user_ctrl_equip` (\
		`uid` int(11) NOT NULL COMMENT '用户id',\
		`eid` int(11) NOT NULL COMMENT '设备id',\
		PRIMARY KEY(`uid`,`eid`),\
			KEY `ctrl_eid` (`eid`),\
				CONSTRAINT `ctrl_eid` FOREIGN KEY(`eid`) REFERENCES `equipment` (`Eid`) ON DELETE CASCADE ON UPDATE CASCADE,\
					CONSTRAINT `ctrl_uid` FOREIGN KEY(`uid`) REFERENCES `ctuser` (`Uid`) ON DELETE CASCADE ON UPDATE CASCADE\
						) ENGINE=InnoDB DEFAULT CHARSET=utf8; ";
	if (false == m_db.Execute(sql)) {
		return false;
	}
	// 用户控制设备表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`ctrl_history` (\
		`cid` int(11) NOT NULL AUTO_INCREMENT COMMENT '控制历史id',\
		`uid` int(11) DEFAULT NULL COMMENT '控制者id',\
		`eid` int(11) DEFAULT NULL COMMENT '被控设备id',\
		`start_time` datetime NOT NULL COMMENT '开始控制时间',\
		PRIMARY KEY(`cid`),\
			KEY `his_ctrl_uid` (`uid`),\
				KEY `his_ctrl_eid` (`eid`),\
					CONSTRAINT `his_ctrl_eid` FOREIGN KEY(`eid`) REFERENCES `equipment` (`Eid`) ON DELETE CASCADE ON UPDATE CASCADE,\
						CONSTRAINT `his_ctrl_uid` FOREIGN KEY(`uid`) REFERENCES `ctuser` (`Uid`) ON DELETE CASCADE ON UPDATE CASCADE\
							) ENGINE=InnoDB DEFAULT CHARSET=utf8; ";
	if (false == m_db.Execute(sql)) {
		return false;
	}
	if (false == m_db.ChangeUser(NULL, NULL, "CrossTime")) {
		return false;
	}
	return true;
}

int CCrossTimeServer::testDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value bodyroot; // 返回数据
	bodyroot["message"] = "success";
	bodyroot["status"] = 1;
	bodyData = bodyroot.toStyledString();
	return 0;
}

int CCrossTimeServer::registerDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	Json::Value bodyroot; // 返回数据
	do {
		bodyroot["message"] = "错误";
		bodyroot["status"] = 2;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			LOG_ERROR << "json 数据解析失败";
			m_logFile->flush();
			break;
		}
		std::string fields[] = { "phone","username","password","balance","question1","answer1","question2", "answer2", "registerTime" };
		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
			LOG_ERROR << "数据字段个数不匹配";
			m_logFile->flush();
			break;
		}
		for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				LOG_ERROR << "要求的字段不存在";
				m_logFile->flush();
				goto BREAK;
			}
			else if ((false == root[fields[i]].isString())
				&& (false == root[fields[i]].isDouble())) {
				// 数据不能为数组类型
				LOG_ERROR << "数据类型不匹配";
				m_logFile->flush();
				goto BREAK;
			}
		}
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "select Uid from ctuser where Uphone='%s';\0\0", root[fields[0]].asString().c_str());
		LOG_INFO << "registerDispose select uid sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 判断当前的手机号码存不存在
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		if (res->GetRecordsSize() > 0) {
			m_db.FreeResult(res);
			// 当前电话号码已经存在
			bodyroot["message"] = "当前手机号已被注册";
			break;
		}
		else {
			m_db.FreeResult(res);
		}
		// 向用户表中插入数据
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"insert into ctuser(uphone, uname, upassword, ubalance, Uregister_time, Uforbidden, Uexpire) VALUES('%s', '%s', '%s', %lf, '%s', 1, 1);\0\0",
			root[fields[0]].asString().c_str(), root[fields[1]].asString().c_str(),
			root[fields[2]].asString().c_str(), root[fields[3]].asDouble(),
			root[fields[8]].asString().c_str()
		);
		LOG_INFO << "registerDispose insert user sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 向数据库中插入数据
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		// 查询用户id
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"select Uid from `ctuser` where Uphone='%s';\0\0",
			root[fields[0]].asString().c_str()
		);
		LOG_INFO << "registerDispose select uid sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 判断当前的手机号码存不存在
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		long long uid;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			uid = rec[0][0].asLONGLONG();*/


			uid = (*res)[0]["Uid"].asLONGLONG();

			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		// 向忘记密码表中插入数据
		snprintf((char*)sql.c_str(), sql.size(), "insert into revert_password(Uid, question1, answer1, question2, answer2) VALUES(%d, '%s', '%s', '%s', '%s');\0\0", uid, root[fields[4]].asString().c_str(), root[fields[5]].asString().c_str(), root[fields[6]].asString().c_str(), root[fields[7]].asString().c_str());
		LOG_INFO << "registerDispose insert revert_password sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 向数据表中添加信息
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		// 向vip表插入用户信息
		snprintf((char*)sql.c_str(), sql.size(), "insert into vip(Uid, status, grade, acc_amount, expire_time, ctrl_time, ctrl_num, surplus_num) VALUES(%d, b'0', 0, 0, '%s', 10, 10, 10);\0\0", uid, root[fields[8]].asString().c_str());
		LOG_INFO << "registerDispose insert vip sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {  // 向数据表中添加信息
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		bodyroot["message"] = "success";
		bodyroot["status"] = 0;
	} while (false);
BREAK:
	bodyData = bodyroot.toStyledString();
	return 0;
}

int CCrossTimeServer::loginDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value body;
	body["message"] = "用户名或密码错误";
	body["status"] = 2;
	do {
		Buffer buffer = parser.Body();
		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			LOG_ERROR << "json 数据解析失败";
			m_logFile->flush();
			break;
		}
		std::string fields[] = { "phone","password","ip" };
		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
			LOG_ERROR << "数据字段个数不匹配";
			m_logFile->flush();
			break;
		}
		for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				LOG_ERROR << "要求的字段不存在";
				m_logFile->flush();
				goto BREAK;
			}
			else if (false == root[fields[i]].isString()) {
				// 数据不能为数组类型
				LOG_ERROR << "数据类型不匹配";
				m_logFile->flush();
				goto BREAK;
			}
		}

		std::string phone = root[fields[0]].asString(); // 手机号
		std::string password = root[fields[1]].asString(); // 密码
		std::string ip = root[fields[2]].asString();   // ip地址

		LOG_INFO << "login: phone [" << phone.c_str() << "] password [" << password.c_str() << "] IP地址 [" << ip.c_str() << "]";
		m_logFile->flush();
		if ((phone == "") || (password == "")) {
			LOG_INFO << "没有数据的登录请求";
			m_logFile->flush();
			break;
		}
		// 数据库的查询
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "select Upassword from ctuser where Uphone='%s';\0\0", phone.c_str());
		LOG_INFO << "loginDispose select password sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 执行sql查找密码
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Buffer upassword;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			upassword = rec[0][0];*/

			upassword = (*res)[0]["Upassword"];

			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		LOG_INFO << "数据库" << phone << "用户密码:[" << upassword.c_str() << "]";
		m_logFile->flush();
		if (password == upassword) {
			// 验证成功
			body["message"] = "success";
			body["status"] = 0;
			break;
		}
	} while (false);
BREAK:
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::bectrlOnlineDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	Json::Value body;
	do {
		body["message"] = "错误";
		body["status"] = 2;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			LOG_ERROR << "json 数据解析失败";
			m_logFile->flush();
			break;
		}
		std::string fields[] = { "equipName", "width", "height","phone","password", "type" };

		if (false == root.isMember(fields[5])) {
			//  没有type字段
			LOG_ERROR << "没有type字段";
			m_logFile->flush();
			break;
		}
		if (false == root[fields[5]].isString()) {
			LOG_ERROR << "type字段不是string类型";
			m_logFile->flush();
			break;
		}

		std::string type = root[fields[5]].asString();

		if (type == "2") {
			// 嵌入式设备
			auto it = m_mapSockAddr.find(clientSocket);
			if (it == m_mapSockAddr.end()) {
				// TODO:有问题，这里面的客户端怎么没有
				LOG_ERROR << "有问题，这里面的客户端怎么没有";
				m_logFile->flush();
				break;
			}
			it->second.isEquip = true;
			it->second.eInfo.equipName = root[fields[0]].asString().c_str();
			it->second.eInfo.width = 0;
			it->second.eInfo.height = 0;
			it->second.eInfo.eKey = "123456";
			it->second.eInfo.eStatus = 1;
			it->second.eInfo.type = 2;
			LOG_INFO << "嵌入式设备上线";
			m_logFile->flush();
		}
		else if (type == "1") {
			auto members = root.getMemberNames();
			if (members.size() < (sizeof(fields) / sizeof(*fields))) {
				break;
			}
			for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
				if (false == root.isMember(fields[i])) {
					// 数据不匹配
					goto BREAK;
				}
				else if ((false == root[fields[i]].isString()) && (false == root[fields[i]].isInt())) {
					// 数据不能为数组类型
					LOG_ERROR << "数据类型不匹配";
					m_logFile->flush();
					goto BREAK;
				}
			}
			// 用户数据库信息验证
			std::string phone = root[fields[3]].asString();
			std::string password = root[fields[4]].asString();
			// 用户判断是否正确
			std::string sql(512, '\0');
			snprintf(
				(char*)sql.c_str(),
				sql.size(),
				"select Upassword from ctuser where Uphone='%s';\0\0",
				phone.c_str()
			);
			LOG_INFO << "bectrlOnlineDispose select upssword sql:[" << sql.c_str() << "]";
			m_logFile->flush();
			if (false == m_db.Execute(sql)) { // 执行sql查找密码
				LOG_ERROR << "sql 执行失败";
				m_logFile->flush();
				body["message"] = "用户名或密码错误";
				break;
			}
			const ResultRecord* res = m_db.GetResult();
			if (res == NULL) {
				break;
			}
			Buffer upassword;
			if (res->GetRecordsSize() > 0) {
				/*Records rec = res->GetRecords();
				upassword = rec[0][0];*/
				upassword = (*res)[0]["Upassword"];
				m_db.FreeResult(res);
			}
			else m_db.FreeResult(res);
			LOG_INFO << "数据库" << phone.c_str() << "用户密码:[" << upassword.c_str() << "]";
			m_logFile->flush();
			if (password != upassword) {
				// 验证失败
				body["message"] = "用户名或密码错误";
				break;
			}
			// 在map里面找到对应的客户端信息
			auto it = m_mapSockAddr.find(clientSocket);
			if (it == m_mapSockAddr.end()) {
				// TODO:有问题，这里面的客户端怎么没有
				LOG_ERROR << "有问题，这里面的客户端怎么没有";
				m_logFile->flush();
				break;
			}
			it->second.isEquip = true;
			it->second.eInfo.equipName = root[fields[0]].asString().c_str();
			it->second.eInfo.width = root[fields[1]].asInt();
			it->second.eInfo.height = root[fields[2]].asInt();
			it->second.eInfo.eKey = "123456";  // TODO:当前设备密钥先这样
			it->second.eInfo.eStatus = 1;  // 当前为空闲状态
			it->second.eInfo.type = 1;
			LOG_INFO << "设备的尺寸比例: width:" << it->second.eInfo.width << "height:" << it->second.eInfo.height;
			m_logFile->flush();
		}
		body["message"] = "success";
		body["status"] = 0;
	} while (false);
BREAK:
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::getAvatarListDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value Item;
	Json::Value arrList;
	Json::Value body;

	const char* imgName[] = {
		"/1.jpg",
		"/2.jpg",
		"/3.jpg",
		"/4.jpg",
		"/5.jpg",
		"/6.jpg",
		"/7.jpg",
		"/8.jpg",
		"/9.jpg",
		"/10.jpg",
	};

	for (int i = 0; i < (int)(sizeof(imgName) / sizeof(*imgName)); i++) {
		Item["id"] = i + 1;
		Item["url"] = imgName[i];
		arrList.append(Item);
	}
	body["imgList"] = arrList;
	body["message"] = "success";
	body["status"] = 0;
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::equipmentlistDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// TODO现在就先给前端设备名和IP地址
	Json::Value root;
	Json::Value arrEquip;
	Json::Value eItem; // 设备的每一项
	long eCount = 0; // 设备的数量
	for (auto it = m_mapSockAddr.begin(); it != m_mapSockAddr.end(); it++) {
		if (it->second.isEquip == true) {
			// 获取IP地址
			// 获取设备名称
			// 获取设备状态
			eItem["id"] = (eCount + 1);
			eItem["name"] = it->second.eInfo.equipName;
			eItem["ip"] = inet_ntoa(it->second.clientAddr.sin_addr);
			eItem["type"] = it->second.eInfo.type;
			eItem["eStatus"] = it->second.eInfo.eStatus;
			arrEquip.append(eItem);
			eCount++;
		}
	}
	root["totalNumber"] = eCount;
	root["equipments"] = arrEquip;
	bodyData = root.toStyledString();
	return 0;
}

//int CCrossTimeServer::startEquipmentCtrlDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
//{
//	// 请求数据
//	Buffer buffer = parser.Body();
//	// 解析请求体数据
//	Json::Value root;
//	Json::Reader reader;
//	Json::Value bodyRoot; // 返回数据的json结构
//	bodyRoot["status"] = 2;
//	bodyRoot["message"] = "执行失败！";
//	do {
//		if (false == reader.parse(buffer, root)) {
//			// 解析失败
//			break;
//		}
//		// 判断body中要求的字段存不存在
//		std::string fields[] = { "ctrlerPhone", "bectrlEquipName", "startCtrlTime", "key" };
//		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
//			break;
//		}
//		for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
//			if (false == root.isMember(fields[i])) {
//				// 数据不匹配
//				goto BREAK;
//			}
//			else if (false == root[fields[i]].isString()) {
//				// 数据类型不匹配
//				LOG_ERROR << "数据类型不匹配";
//				m_logFile->flush();
//				goto BREAK;
//			}
//		}
//		std::string eName = root[fields[1]].asString(); // 设备名称
//		std::string phone = root[fields[0]].asString(); // 控制者手机号
//		std::string scTime = root[fields[2]].asString(); // 开始控制时间
//		std::string key = root[fields[3]].asString();    // 设备密钥
//		if (phone == "") {
//			// 手机号没有，是不可以监控的
//			break;
//		}
//		// 获取控制用户的id
//		std::string sql(512, '\0');
//		snprintf(
//			(char*)sql.c_str(),
//			sql.size(),
//			"select Uid from ctuser where Uphone='%s';\0\0",
//			phone.c_str()
//		);
//		LOG_INFO << "startEquipmentCtrlDispose select uid sql:[" << sql.c_str() << "]";
//		m_logFile->flush();
//		if (false == m_db.Execute(sql)) { // 执行sql查找用户id
//			LOG_ERROR << "sql 执行失败";
//			m_logFile->flush();
//			break;
//		}
//		const ResultRecord* res = m_db.GetResult();
//		if (res == NULL) {
//			break;
//		}
//		Buffer id;
//		if (res->GetRecordsSize() > 0) {
//			/*Records rec = res->GetRecords();
//			id = rec[0][0];*/
//
//			id = (*res)[0]["Uid"];
//			m_db.FreeResult(res);
//		}
//		else {
//			m_db.FreeResult(res);
//			break;
//		}
//		if (id == "") {
//			// 查询不到用户的id
//			break;
//		}
//		// 设备查找
//		for (auto it = m_mapSockAddr.begin(); it != m_mapSockAddr.end(); it++) {
//			if (it->second.isEquip == true) {
//				if (it->second.eInfo.equipName == eName) {
//					if (it->second.eInfo.eStatus == 2) {
//						// 当前设备为被控制状态
//						goto BREAK;
//					}
//					else {
//						// 可以控制当前设备
//						// 首先进行密钥匹配
//						if (it->second.eInfo.eKey != key) {
//							// 密钥不匹配
//							goto BREAK;
//						}
//
//						// 本次控制存放在服务器的图片名 (控制者id-设备名)
//						std::string iDir = id + "-" + eName;
//
//						auto ret = m_mbEqu.find(eName);
//						if (ret != m_mbEqu.end()) { // 背控制的设备信息中有这个设备
//							goto BREAK;
//						}
//
//						// 在这先给被控端创建好，发送图片的位置和套接字
//						// 套接字、端口、控制者id、图片的保存位置
//						SUPEQUIPINFO SEInfo = {};
//						SEInfo.recvImagePort = FILESERVER_PORT;
//						SEInfo.imageDir = ("/var/www/example/img/" + iDir + ".jpeg").c_str();
//						SEInfo.ctrlerId = id.c_str();  // 监控者的id
//
//						// 将这个数据插入到映射表中
//						auto insRet = m_mbEqu.insert(std::pair<std::string, SUPEQUIPINFO>(eName, SEInfo));
//						if (!insRet.second) {
//							// 添加失败
//							goto BREAK;
//						}
//
//						// 这里就是用户id-设备id
//						// 先确定图片文件的命名
//						// 然后将这个名称告诉被控端
//						// 然后将这个告诉控制端
//
//						// 向被控端发送开始控制数据包
//
//						CPacket pack(CTC_STARTEQUIPCTRL, FILESERVER_PORT, iDir.c_str(), iDir.size());
//						// 向客户端发送控制消息
//						ssize_t len = send(it->first, pack.outStr(), pack.Size(), 0);
//						if (len <= 0) {
//							m_mbEqu.erase(insRet.first);
//							LOG_ERROR << "数据发送失败";
//							m_logFile->flush();
//							goto BREAK;
//						}
//						// TODO:接收客户端的应答包，并设置超时处理，超过事件没有数据
//
//
//						it->second.eInfo.eStatus = 2;
//
//						bodyRoot["status"] = 1;
//						bodyRoot["message"] = "success";
//						bodyRoot["imageDir"] = iDir.c_str();
//						bodyRoot["CtrlTime"] = 5;
//						bodyRoot["width"] = it->second.eInfo.width;
//						bodyRoot["height"] = it->second.eInfo.height;
//						goto BREAK;
//					}
//				}
//			}
//		}
//	} while (false);
//BREAK:
//	bodyData = bodyRoot.toStyledString();
//	return 0;
//}

int CCrossTimeServer::startEquipmentCtrlDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	Json::Value bodyRoot; // 返回数据的json结构
	do {
		bodyRoot["eStatus"] = 2;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			break;
		}
		std::string fields[] = { "ctrlerPhone", "bectrlEquipName", "startCtrlTime", "key" };
		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
			break;
		}
		for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				goto BREAK;
			}
			else if (false == root[fields[i]].isString()) {
				// 数据类型不匹配
				LOG_ERROR << "数据类型不匹配";
				m_logFile->flush();
				goto BREAK;
			}
		}
		std::string eName = root[fields[1]].asString(); // 设备名称
		std::string phone = root[fields[0]].asString(); // 控制者手机号
		std::string scTime = root[fields[2]].asString(); // 开始控制时间
		std::string key = root[fields[3]].asString();    // 设备密钥
		if (phone == "") {
			// 手机号没有，是不可以监控的
			break;
		}
		// 获取控制用户的id
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "select Uid from ctuser where Uphone='%s';\0\0", phone.c_str());
		LOG_INFO << "startEquipmentCtrlDispose select uid sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 执行sql查找用户id
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Buffer id;
		if (res->GetRecordsSize() > 0) {
			Records rec = res->GetRecords();
			id = rec[0][0];
			m_db.FreeResult(res);
		}
		else {
			m_db.FreeResult(res);
			break;
		}
		if (id == "") {
			// 查询不到用户的id
			break;
		}
		// 设备查找
		for (auto it = m_mapSockAddr.begin(); it != m_mapSockAddr.end(); it++) {
			if (it->second.isEquip == true) {
				if (it->second.eInfo.equipName == eName) {
					if (it->second.eInfo.eStatus == 2) {
						// 当前设备为被控制状态
						break;
					}
					else {
						// 可以控制当前设备
						// 首先进行密钥匹配
						if (it->second.eInfo.eKey != key) {
							// 密钥不匹配
							break;
						}

						std::string iDir = "user1-"; // TODO：图片拼接就是 用户id-设备id
						iDir += eName;

						auto ret = m_mbEqu.find(eName);
						if (ret != m_mbEqu.end()) { // 当前设备已经被控制
							break;
						}
						// 在这先给被控端创建好，发送图片的位置和套接字
						// 套接字、端口、控制者id、图片的保存位置
						SUPEQUIPINFO SEInfo = {};
						SEInfo.recvImagePort = 9999;
						SEInfo.imageDir = ("/var/www/example/img/" + iDir + ".jpeg").c_str();
						SEInfo.ctrlerId = id.c_str();  // 监控者的id
						// 创建接收客户端图片消息的套接字
						int sock = socket(AF_INET, SOCK_STREAM, 0);
						if (sock < 0) {
							// 套接字创建失败
							break;
						}
						sockaddr_in recvImageAddr = {};
						recvImageAddr.sin_family = AF_INET;
						recvImageAddr.sin_port = htons(SEInfo.recvImagePort);
						recvImageAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
						// 设置允许端口复用
						if (!CTools::SetPortReuse(sock)) {
							LOG_ERROR << "重用端口设备失败";
							m_logFile->flush();
							close(sock);
							break;
						}
						// 绑定端口
						if (bind(sock, (sockaddr*)&recvImageAddr, sizeof(recvImageAddr)) < 0) {
							LOG_ERROR << "服务器端口绑定失败 " << SEInfo.recvImagePort;
							m_logFile->flush();
							close(sock);
							break;
						}
						// 监听端口
						if (listen(sock, 1) < 0) {
							LOG_ERROR << "服务器套接字监听失败";
							m_logFile->flush();
							close(sock);
							break;
						}
						SEInfo.recvImageSock = sock;
						// 将这个数据插入到映射表中
						m_mbEqu.insert(std::pair<std::string, SUPEQUIPINFO>(eName, SEInfo));

						// 通知被控端向这个指定端口发送图片数据【在线程里面通知吧】
						pthread_t thread;
						screenArgs* pArgs = new screenArgs(this, it->first, eName);
						if (pthread_create(&thread, NULL, &CCrossTimeServer::ScreenMonitorThreadEnt, pArgs) != 0) {
							// 线程创建失败
							delete pArgs;
							break;
						}
						// 线程开启成功
						bodyRoot["eStatus"] = 1;
						bodyRoot["imageDir"] = iDir.c_str();
						bodyRoot["CtrlTime"] = 10;
						bodyRoot["width"] = it->second.eInfo.width;
						bodyRoot["height"] = it->second.eInfo.height;
						it->second.eInfo.eStatus = 2;
						it->second.eInfo.thread = thread;
					}
				}
			}
		}
	} while (false);
BREAK:
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::controlEquipDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// 请求体
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	Json::Value bodyRoot; // 返回数据的json结构
	bodyRoot["message"] = "操作失败";
	bodyRoot["status"] = 2;
	do {
		// 解析请求体数据
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			break;
		}
		std::string fields[] = { "ctrlerPhone", "type", "list" };
		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
			break;
		}
		for (int i = 0; i<int(sizeof(fields) / sizeof(*fields)); i++) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				goto BREAK;
			}
			if ((false == root[fields[i]].isString())
				&& (false == root[fields[i]].isInt())
				&& (false == root[fields[i]].isArray())) {
				// 数据类型不匹配
				LOG_ERROR << "数据类型不匹配";
				m_logFile->flush();
				goto BREAK;
			}
		}
		if (false == root[fields[0]].isString()) {
			// 数据类型不匹配
			LOG_ERROR << "数据类型不匹配";
			m_logFile->flush();
			goto BREAK;
		}
		std::string phone = root[fields[0]].asString();  // 手机号
		if (phone == "") {
			// 信息不全
			break;
		}
		if (false == root[fields[1]].isInt()) {
			// 数据类型不匹配
			LOG_ERROR << "数据类型不匹配";
			m_logFile->flush();
			goto BREAK;
		}
		int type = root[fields[1]].asInt();

		if (root[fields[2]].isArray() == false) {
			break;
		}
		for (int i = 0; i < (int)root[fields[2]].size(); i++) {
			std::string eName = root[fields[2]][i]["bectrlEquipName"].asString();
			std::string timeout = root[fields[2]][i]["timeout"].asString();
			// 首先查看这个设备在不在线
			SOCKET key = findEInfo(eName);
			if (key == -1) {
				// 表示这台设备不存在
				continue;
			}
			// 在上数据库中查询当前用户对这台设备有没有操作权限
			// SELECT COUNT(uid) as count FROM user_ctrl_equip WHERE uid = (SELECT Uid FROM ctuser WHERE Uphone = '15111111111') AND eid = (SELECT Eid FROM equipment WHERE Ename = '1');
			// 
			/*****数据库校验判断之后再加***/
			std::string sql(512, '\0');
			//snprintf(
			//	(char*)sql.c_str(),
			//	sql.size(),
			//	"SELECT COUNT(uid) as count FROM user_ctrl_equip WHERE uid = (SELECT Uid FROM ctuser WHERE Uphone = '%s') AND eid = (SELECT Eid FROM equipment WHERE Ename = '%s');\0\0",
			//	phone.c_str(),
			//	eName.c_str()
			//);
			//LOG_INFO << "controlEquipDispose select user controllable authority sql:[" << sql.c_str() << "]";
			//m_logFile->flush();
			//if (false == m_db.Execute(sql)) { // 执行sql查找用户id
			//	LOG_ERROR << "sql 执行失败";
			//	m_logFile->flush();
			//	break;
			//}
			//const ResultRecord* res = m_db.GetResult();
			//if (res == NULL) {
			//	continue;
			//}
			//int count = 0;
			//if (res->GetRecordsSize() > 0) {
			//	/*Records rec = res->GetRecords();
			//	count = (int)rec[0][0].asLONGLONG();*/
			//	count = (int)(*res)[0]["count"].asLONGLONG();
			//	m_db.FreeResult(res);
			//	if (count <= 0) {
			//		// 表示用户没有对其的控制权限
			//		LOG_ERROR << "改用户没有对改设备的控制能力";
			//		m_logFile->flush();
			//		continue;
			//	}
			//}
			//else {
			//	m_db.FreeResult(res);
			//	continue;
			//}
			// 向被控端发送关机的服务请求，并将被控端信息在设备映射表中移除


			LOG_ERROR << "type 类型:" << type;
			m_logFile->flush();

			switch (type)
			{
			case 1:  // 关机
			case 2:  // 重启
			{
				LOG_ERROR << "关机重启命令触发";
				m_logFile->flush();

				unsigned int cmd = (type == 1) ? CTC_EQUIPSHUTDOWN : CTC_EQUIPRESTART;
				// 向设备发送关机或重启操作
				CPacket pack((unsigned short)cmd, cmd, timeout.c_str(), timeout.size());
				ssize_t len = send(key, pack.outStr(), pack.Size(), 0);
				if (len <= 0) {
					LOG_ERROR << "数据发送失败";
					m_logFile->flush();
				}
				//// 控制设备信息
				//if (m_mbEqu.find(eName) != m_mbEqu.end()) {
				//	m_mbEqu.erase(eName);
				//}
				//// 在线设备信息
				//if (m_mapSockAddr.find(key) != m_mapSockAddr.end()) {
				//	m_mapSockAddr.erase(key);
				//}
			}
			break;
			case 3:  // 删除
			{
				LOG_ERROR << "删除命令触发";
				m_logFile->flush();
				// TODO:向客户端发送退出程序的请求

				// 在数据库中将这个设备删除
				// 先找出设备的id  SELECT Eid FROM equipment WHERE Ename='1';
				// 删除设备  DELETE FROM equipment WHERE Eid='1';
				snprintf((char*)sql.c_str(), sql.size(), "SELECT Eid FROM equipment WHERE Ename='%s';\0\0", eName.c_str());
				LOG_INFO << "controlEquipDispose select equip id sql:[" << sql.c_str() << "]";
				m_logFile->flush();
				if (false == m_db.Execute(sql)) { // 执行sql查找用户id
					LOG_ERROR << "sql 执行失败";
					m_logFile->flush();
					break;
				}
				const ResultRecord* res = m_db.GetResult();
				if (res == NULL) {
					break;
				}
				std::string eId;
				if (res->GetRecordsSize() > 0) {
					/*Records rec = res->GetRecords();
					eId = rec[0][0];*/

					eId = (*res)[0]["Eid"];
					m_db.FreeResult(res);
				}
				else {
					m_db.FreeResult(res);
					break;
				}
				if (eId == "") {
					break;
				}
				snprintf((char*)sql.c_str(), sql.size(), "DELETE FROM equipment WHERE Eid='%s';\0\0", eId.c_str());
				LOG_INFO << "controlEquipDispose delete equip sql:[" << sql.c_str() << "]";
				m_logFile->flush();
				if (false == m_db.Execute(sql)) { // 执行sql查找用户id
					LOG_ERROR << "sql 执行失败";
					m_logFile->flush();
					break;
				}
				if (m_mbEqu.find(eName) != m_mbEqu.end()) {
					m_mbEqu.erase(eName);
				}
				if (m_mapSockAddr.find(key) != m_mapSockAddr.end()) {
					m_mapSockAddr.erase(key);
				}
			}
			break;
			}
		}
		bodyRoot["message"] = "success";
		bodyRoot["status"] = 1;
	} while (false);
BREAK:
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::dataSummaryDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value body;
	body["message"] = "获取失败";
	body["status"] = 2;
	do {
		// 需要先验证TOKEN
		// 加判断，看当前发起关闭请求的用户是不是开启控制的用户
		auto phoneIt = parser.Headers().find("Authorization");
		if (phoneIt == parser.Headers().end()) {
			phoneIt = parser.Headers().find("authorization");
			if (phoneIt == parser.Headers().end()) {
				LOG_ERROR << "指定字段不存在（token）";
				m_logFile->flush();
				break;
			}
		}
		std::string phone = phoneIt->second;
		body["visitTotal"] = 386675;
		Json::Value arrWeekDay;
		arrWeekDay[0] = 4996;
		arrWeekDay[1] = 2511;
		arrWeekDay[2] = 1896;
		arrWeekDay[3] = 4859;
		arrWeekDay[4] = 2541;
		arrWeekDay[5] = 3645;
		arrWeekDay[6] = 2547;
		body["weekPerDayTotal"] = arrWeekDay;
		// SELECT COUNT(Uid) as count FROM ctuser;
		std::string sql(512, '\0');
		/*
		* // 真实数据
		snprintf((char*)sql.c_str(), sql.size(), "SELECT COUNT(Uid) as count FROM ctuser;\0\0");
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		if (res->GetRecordsSize() > 0) {
			Records rec = res->GetRecords();
			body["registerCount"] = (int)rec[0][0].asLONGLONG();
			m_db.FreeResult(res);
		}
		else {
			LOG_ERROR << "数据获取失败";
			m_logFile->flush();
			m_db.FreeResult(res);
			break;
		}*/
		// 虚拟数据
		body["registerCount"] = 15632;

		struct logHistory
		{
			std::string ipAddr;
			std::string loginTime;
			std::string address;
		};

		logHistory lh[] = {
			{"19.12.14.25", "2024-05-11 17:08:52", "山西省太原市"},
			{"100.25.36.153", "2024-05-17 13:50:07", "河北省石家庄市"},
			{"120.32.25.16","2024-05-17 13:51:05", "青海省海东市"},
			{"110.110.110.110", "2024-05-17 13:53:03", "贵州省贵阳市"},
			{"81.70.91.154","2024-05-17 13:54:26", "西藏自治区拉萨市"},
			{"64.58.93.146", "2024-06-08 13:56:17", "西藏自治区拉萨市"},
			{"85.26.95.146", "2024-06-08 13:56:17", "四川省攀枝花市"},
			{"81.70.91.154","2024-05-17 13:54:26", "贵州省贵阳市"},
			{"64.58.93.146", "2024-06-08 13:56:17", "湖南省衡阳市"},
			{"85.26.95.146", "2024-06-08 13:56:17", "山东省威海市"}
		};

		Json::Value arrNotices; // 数组
		Json::Value Item;       // 每一个节点

		for (int i = 0; i < (int)(sizeof(lh) / sizeof(*lh)); i++) {
			Item["ip"] = lh[i].ipAddr;
			Item["date"] = lh[i].loginTime;
			Item["address"] = lh[i].address;
			arrNotices.append(Item);
		}


		// 查找指定用户的登录信息（真实数据）
		// SELECT ipAddr, loginTime, longitude, latitude FROM ulogin_history WHERE Uid=(SELECT Uid FROM ctuser WHERE Uphone='15046543174') LIMIT 10;
		/*snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"SELECT ipAddr, loginTime, address FROM ulogin_history WHERE Uid = (SELECT Uid FROM ctuser WHERE Uphone = '%s') LIMIT 30;\0\0",
			phone.c_str()
		);
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			body.clear();
			body["message"] = "获取失败";
			body["status"] = 2;
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			body.clear();
			body["message"] = "获取失败";
			body["status"] = 2;
			break;
		}
		size_t recSize = res->GetRecordsSize();
		if (recSize > 0) {
			Records rec = res->GetRecords();
			for (size_t i = 0; i < recSize; i++) {

				Item["ip"] = (*res)[i]["ipAddr"];
				Item["date"] = (*res)[i]["loginTime"];
				Item["address"] = (*res)[i]["address"];
				arrNotices.append(Item);
			}
			m_db.FreeResult(res);
		}
		else {
			LOG_ERROR << "数据获取失败";
			m_logFile->flush();
			m_db.FreeResult(res);
		}*/



		body["list"] = arrNotices;
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::currentOnlineDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	static int online = 2000;
	Json::Value body;
	body["message"] = "success";
	body["status"] = 1;
	body["online"] = online + rand() % 10;
	online += (rand() % 2 == 0 ? (-1 * (rand() % 5)) : (rand() % 5));
	online += rand() % 5;
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::getWeatherDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value body;
	body["message"] = "获取失败";
	body["status"] = 2;
	do {
		Buffer ip = url["ip"];
		LOG_INFO << "ip地址为：" << ip.c_str();
		m_logFile->flush();
		if (ip == "") {
			break;
		}
		std::string url;
		std::map<std::string, std::string> params;
		params["ip"] = ip.c_str();
		params["coor"] = "bd09ll";
		params["ak"] = "kEmx0DECl6ZywbdDBRbhxJLiIOX7It4P";
		std::string ack = CTools::CurlTool("http://api.map.baidu.com", "/location/ip", params, url);

		// 解析请求体数据
		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(ack, root)) {
			// 解析失败
			LOG_ERROR << "baidu api数据解析失败 url:[" << url.c_str() << "] ack:[" << ack << "]";
			m_logFile->flush();
			break;
		}
		int status = root["status"].asInt();
		if (status != 0) {
			LOG_ERROR << "应答包状态码不为0 ack:[" << ack << "]";
			m_logFile->flush();
			break;
		}

		// std::string con_add_det = root["content"]["address_detail"]["city"].asString();   // 这个是市
		// http://api.lolimi.cn/API/weather/?city=北京市
		params.clear();
		params["city"] = root["content"]["address_detail"]["city"].asString();

		ack = CTools::CurlTool("http://api.lolimi.cn", "/API/weather/", params, url);

		LOG_INFO << " 天气API url:" << url.c_str();
		m_logFile->flush();

		bodyData = ack.c_str();
		return 0;
	} while (false);
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::bectrlDownlineDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求数据
	Json::Value  root;
	Json::Reader reader;
	Json::Value  body;
	do {
		body["message"] = "错误";
		body["status"] = 2;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			LOG_ERROR << "json 数据解析失败";
			m_logFile->flush();
			break;
		}
		std::string fields[] = { "equipName" };
		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
			break;
		}
		for (int i = 0; i<int(sizeof(fields) / sizeof(*fields)); i++) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				LOG_ERROR << "数据不匹配, 字段不存在";
				m_logFile->flush();
				goto BREAK;
			}
			if ((false == root[fields[i]].isString())) {
				LOG_ERROR << "数据类型不匹配";
				m_logFile->flush();
				goto BREAK;
			}
		}
		// 设备名
		std::string EName = root[fields[0]].asString();
		if (EName != "")return -1;
	} while (false);
BREAK:
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::getEquipAllDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// SELECT Ename, longitude, latitude FROM equipment;
	Json::Value body;
	body["message"] = "获取失败";
	body["status"] = 2;

	do {
		std::string sql(512, '\0');
		// SELECT Eid, Ename, longitude, latitude FROM equipment LIMIT 999999  OFFSET 1;
		// SELECT Eid, Ename, longitude, latitude FROM equipment ORDER BY Eid;
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"SELECT Eid, Ename, longitude, latitude FROM equipment;\0\0"
		);
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Json::Value arrNotices; // 数组
		Json::Value Item;       // 每一个节点
		size_t recSize = res->GetRecordsSize();

		Item["eName"] = "a";
		Item["llitude"] = "126.652713,45.743479"; // 126.652713,45.743479
		Item["status"] = 3;
		arrNotices.append(Item);

		if (recSize > 0) {
			// Records rec = res->GetRecords();
			for (size_t i = 0; i < recSize; i++) {
				std::string eName = (*res)[i]["Ename"];
				Item["eName"] = eName;
				std::string ll = (*res)[i]["longitude"];  // 经纬度
				ll += ("," + (*res)[i]["latitude"]);
				Item["llitude"] = ll;
				auto ret = findEInfo(eName);
				if (ret == -1) {
					// 设备不在线
					// Item["status"] = 0;
					if (ll == "122.124391,37.519933") {
						// 这个是设备，正在被控制
						Item["status"] = 2;
					}
					else if (ll == "126.652713,45.743479") {
						Item["status"] = 3;
					}
					else
						Item["status"] = rand() % 3;
				}
				else {
					auto it = m_mapSockAddr.find(ret);
					Item["status"] = it->second.eInfo.eStatus;
				}
				arrNotices.append(Item);
			}
			m_db.FreeResult(res);
		}
		else {
			LOG_ERROR << "数据获取失败";
			m_logFile->flush();
			m_db.FreeResult(res);
			break;
		}
		body["list"] = arrNotices;
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
	bodyData = body.toStyledString();
	return 0;
}




int CCrossTimeServer::endEquipmentCtrlDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	Json::Value bodyRoot;   // 返回数据的json结构
	bodyRoot["message"] = "执行失败";
	bodyRoot["status"] = 2;
	do {
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			break;
		}
		std::string fields[] = { "bectrlEquipName" };
		if (root.getMemberNames().size() < sizeof(fields) / sizeof(*fields)) {
			break;
		}
		for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				goto BREAK;
			}
			else if (false == root[fields[i]].isString()) {
				// 数据类型不匹配
				LOG_ERROR << "数据类型不匹配";
				m_logFile->flush();
				goto BREAK;
			}
		}

		std::string eName = root[fields[0]].asString();
		// 设备查找
		for (auto it = m_mapSockAddr.begin(); it != m_mapSockAddr.end(); it++) {
			if (it->second.isEquip == true) {
				if (it->second.eInfo.equipName == eName) {
					if (it->second.eInfo.eStatus == 1) {
						// 当前设备没被控制
						goto BREAK;
					}
					else {
						// 停止设备控制状态
						auto ret = m_mbEqu.find(eName);
						if (ret == m_mbEqu.end()) {
							it->second.eInfo.eStatus = 1;
							LOG_WARN << "没有当前设备的控制信息 设备名:" << eName;
							m_logFile->flush();
							goto BREAK;
						}
						// 加判断，看当前发起关闭请求的用户是不是开启控制的用户
						auto phoneIt = parser.Headers().find("Authorization");
						if (phoneIt == parser.Headers().end()) {
							phoneIt = parser.Headers().find("authorization");
							if (phoneIt == parser.Headers().end()) {
								LOG_ERROR << "指定字段不存在（token）";
								m_logFile->flush();
								goto BREAK;
							}
						}
						std::string phone = phoneIt->second;
						std::string sql(512, '\0');
						snprintf(
							(char*)sql.c_str(),
							sql.size(),
							"select Uid from ctuser where Uphone='%s';\0\0",
							phone.c_str()
						);
						LOG_INFO << "endEquipmentCtrlDispose select uid sql:[" << sql.c_str() << "]";
						m_logFile->flush();
						if (false == m_db.Execute(sql)) { // 执行sql查找用户id
							LOG_ERROR << "sql 执行失败";
							m_logFile->flush();
							goto BREAK;
						}
						const ResultRecord* res = m_db.GetResult();
						if (res == NULL) {
							goto BREAK;
						}
						Buffer id;
						if (res->GetRecordsSize() > 0) {
							id = (*res)[0]["Uid"];
							m_db.FreeResult(res);
						}
						else
							m_db.FreeResult(res);
						if (id == "") {
							// 查询不到用户的id
							LOG_ERROR << "查询不到请求发出者信息";
							m_logFile->flush();
							goto BREAK;
						}
						if (ret->second.ctrlerId != id) {
							// 停止控制设备和开启控制设备的用户不一样
							LOG_ERROR << "停止控制设备和开启控制设备的用户不一样";
							m_logFile->flush();
							goto BREAK;
						}
						// 通知被控端停止屏幕数据发送
						char str[] = "停止控制";
						CPacket pack(CTC_ENDEQUIPCTRL, 2, str, sizeof(str));
						ssize_t len = send(it->first, pack.outStr(), pack.Size(), 0);
						if (len <= 0) {
							// TODO:这里可能要强制结束客户端（被控端）

							LOG_ERROR << "数据发送失败";
							m_logFile->flush();
							// goto BREAK;
						}

						void* retval = NULL;
						timespec abs = {};
						abs.tv_sec = 2;
						if (ETIMEDOUT == pthread_timedjoin_np(it->second.eInfo.thread, &retval, &abs)) {
							// 等待线程结束超时
							LOG_ERROR << "等待线程结束超时，以强制关闭监控线程";
							m_logFile->flush();
							pthread_cancel(it->second.eInfo.thread);  // 强制关闭线程
							close(ret->second.sendImageClientSock);
							close(ret->second.recvImageSock);
						}

						// 停止设备控制
						it->second.eInfo.eStatus = 1;
						it->second.eInfo.thread = -1;
						m_mbEqu.erase(ret);

						LOG_INFO << "已停止屏幕监控";
						m_logFile->flush();
						bodyRoot["message"] = "执行成功";
						bodyRoot["status"] = 1;
						goto BREAK;
					}
				}
			}
		}
	} while (false);
BREAK:
	bodyData = bodyRoot.toStyledString().c_str();
	return 0;
}

int CCrossTimeServer::ctrlEventDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value bodyRoot;
	bodyRoot["message"] = "操作失败";
	bodyRoot["status"] = 2;
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	if (false == reader.parse(buffer, root)) {
		// 解析失败
		bodyData = bodyRoot.toStyledString();
		return -1;
	}


	std::string fields[] = { "bectrlEquipName", "EventType", "MousePoint","operation","Value" };
	auto members = root.getMemberNames();
	if (members.size() < sizeof(fields) / sizeof(*fields)) {
		bodyData = bodyRoot.toStyledString();
		return -2;
	}
	for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
		if (false == root.isMember(fields[i])) {
			// 数据不匹配
			bodyData = bodyRoot.toStyledString();
			return -3;
		}
	}
	std::string eName = root[fields[0]].asString();
	for (auto it = m_mapSockAddr.begin(); it != m_mapSockAddr.end(); it++) {
		if (it->second.isEquip == true) {
			if (it->second.eInfo.equipName == eName) {
				if (it->second.eInfo.eStatus == 1) {
					// 当前设备没被控制
					bodyRoot["message"] = "当前设备没被控制";
					bodyRoot["status"] = -1;
				}
				else { // 当前设备正在被控制
					MKEvent MkEvent = {};
					MkEvent.EType = (WORD)root[fields[1]].asInt();
					const Json::Value Point = root[fields[2]];
					MkEvent.MouseXY.x = Point["x"].asInt();
					MkEvent.MouseXY.y = Point["y"].asInt();
					MkEvent.MKOperation = (WORD)root[fields[3]].asInt();
					MkEvent.MKKeyValue = (DWORD)root[fields[4]].asInt();
					char str[sizeof(MKEvent)] = "";
					memcpy(str, &MkEvent, sizeof(MKEvent));
					CPacket pack(CTC_CTRLEVENT, 3, str, sizeof(str));
					long long len = send(it->first, pack.outStr(), pack.Size(), 0);
					if (len <= 0) {
						LOG_ERROR << "数据发送失败";
						m_logFile->flush();
						break;
					}
				}
			}
		}
	}
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::forgetPasswordDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	if (false == reader.parse(buffer, root)) {
		// 解析失败
		return -1;
	}
	Json::Value bodyRoot; // 返回的json数据
	std::string fields[] = { "phone" };
	auto members = root.getMemberNames();
	if (members.size() < sizeof(fields) / sizeof(*fields)) {
		return -2;
	}
	for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
		if (false == root.isMember(fields[i])) {
			// 数据不匹配
			return -3;
		}
	}
	do {
		// 先通过手机号在数据库中查找用户的id
		std::string phone = root[fields[0]].asString();
		// 数据库操作
		std::string sql(512, '\0');
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"select Uid from ctuser where Uphone='%s';\0\0",
			phone.c_str()
		);
		LOG_INFO << "forgetPasswordDispose select uid sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			bodyRoot["message"] = "该用户不存在";
			bodyRoot["status"] = -2;
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		long uid = -1;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			uid = rec[0][0].asLONGLONG();*/

			uid = (*res)[0]["Uid"].asLONGLONG();

			m_db.FreeResult(res);
		}
		else {
			LOG_ERROR << "数据获取失败";
			m_logFile->flush();
			bodyRoot["message"] = "该用户不存在";
			bodyRoot["status"] = -2;
			break;
		}
		// 然后去忘记密码表中查询用户输入的两个问题
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"select question1, question2 from revert_password where Uid=%ld;\0\0",
			uid
		);
		LOG_INFO << "forgetPasswordDispose select revert_password sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			bodyRoot["message"] = "该用户不存在";
			bodyRoot["status"] = -2;
			break;
		}
		res = m_db.GetResult();
		std::string answer1, answer2;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			answer1 = rec[0][0];
			answer2 = rec[0][1];*/

			answer1 = (*res)[0]["question1"];
			answer2 = (*res)[0]["question2"];

			m_db.FreeResult(res);
		}
		else {
			LOG_INFO << "数据获取失败";
			m_logFile->flush();
			bodyRoot["message"] = "该用户不存在";
			bodyRoot["status"] = -2;
			break;
		}

		bodyRoot["message"] = "获取成功";
		bodyRoot["status"] = 1;
		bodyRoot["question1"] = answer1.c_str();
		bodyRoot["question2"] = answer2.c_str();

	} while (false);
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::verifyAnswerDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	if (false == reader.parse(buffer, root)) {
		// 解析失败
		return -1;
	}
	Json::Value bodyRoot; // 返回的json数据
	std::string fields[] = { "phone","answer1","answer2" };
	auto members = root.getMemberNames();
	if (members.size() < sizeof(fields) / sizeof(*fields)) {
		return -2;
	}
	for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
		if (false == root.isMember(fields[i])) {
			// 数据不匹配
			return -3;
		}
	}
	do {
		bodyRoot["message"] = "验证失败";
		bodyRoot["status"] = -2;
		std::string phone = root[fields[0]].asString();
		std::string answer1 = root[fields[1]].asString();
		std::string answer2 = root[fields[2]].asString();
		// 先通过手机号去用户表找到用户id和密码
		std::string sql(512, '\0');
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"select Uid, Upassword from ctuser where Uphone='%s';\0\0",
			phone.c_str()
		);
		LOG_INFO << "verifyAnswerDispose select user sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		long uid = -1;
		std::string password;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			uid = rec[0][0].asLONGLONG();
			password = rec[0][1];*/

			uid = (*res)[0]["Uid"].asLONGLONG();
			password = (*res)[0]["Upassword"];

			m_db.FreeResult(res);
		}
		else {
			LOG_ERROR << "数据获取失败";
			m_logFile->flush();
			break;
		}
		// 然后通过用户id去找设定的两个问题的答案
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"select answer1, answer2 from revert_password where Uid=%ld;\0\0",
			uid
		);
		LOG_INFO << "verifyAnswerDispose select revert_password sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		res = m_db.GetResult();
		std::string std_answer1, std_answer2;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			std_answer1 = rec[0][0];
			std_answer2 = rec[0][1];*/

			std_answer1 = (*res)[0]["answer1"];
			std_answer2 = (*res)[0]["answer2"];

			m_db.FreeResult(res);
		}
		if ((answer1 == std_answer1) && (answer2 == std_answer2)) {
			bodyRoot["message"] = "验证成功";
			bodyRoot["status"] = 1;
			bodyRoot["password"] = password;
			break;
		}
	} while (false);
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::getUserInfoDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value bodyRoot; // 返回的json数据
	do {
		bodyRoot["message"] = "获取失败";
		bodyRoot["status"] = -2;
		std::map<Buffer, Buffer> headers = parser.Headers();
		std::map<Buffer, Buffer>::iterator itt = headers.find("Authorization");
		if (itt == headers.end()) {
			itt = headers.find("authorization");
			if (itt == headers.end()) {
				// 没有token数据
				LOG_ERROR << "信息获取失败";
				m_logFile->flush();
				break;
			}
		}
		std::string phone = itt->second.c_str();
		// 从数据库中获取用户的信息
		std::string sql(512, '\0');
		// select Uname, Uavatar, Upassword, Usex, Uaddr, Uregister_time, Ubalance, ctrl_num, expire_time,grade,acc_amount from ctuser,vip where ctuser.Uid=vip.Uid AND Uphone = '15046543174';
		snprintf((char*)sql.c_str(), sql.size(), "select Uname, Uavatar, Upassword, Usex, Uaddr, Uregister_time, Ubalance, ctrl_num, expire_time,grade,acc_amount from ctuser,vip where ctuser.Uid=vip.Uid AND Uphone = '%s' LIMIT 1;\0\0", phone.c_str());
		LOG_INFO << "getUserInfoDispose select user sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		if (res->GetRecordsSize() > 0) {
			Records rec = res->GetRecords();


			// Uname, Uavatar, Upassword, Usex, Uaddr, Uregister_time, Ubalance, ctrl_num, expire_time,grade,acc_amount

			bodyRoot["name"] = (*res)[0]["Uname"];
			bodyRoot["avatar"] = (*res)[0]["Uavatar"];
			bodyRoot["password"] = (*res)[0]["Upassword"];
			bodyRoot["sex"] = (*res)[0]["Usex"];
			bodyRoot["addr"] = (*res)[0]["Uaddr"];
			bodyRoot["phone"] = phone;
			bodyRoot["registerTime"] = (*res)[0]["Uregister_time"];
			bodyRoot["balance"] = (*res)[0]["Ubalance"].asDOUBLE();
			bodyRoot["equipCtrlCount"] = (*res)[0]["ctrl_num"];
			bodyRoot["expireTime"] = (*res)[0]["expire_time"];
			bodyRoot["vipgrade"] = (*res)[0]["grade"];
			bodyRoot["accBalance"] = (*res)[0]["acc_amount"];
			m_db.FreeResult(res);
		}
		else {
			LOG_ERROR << "数据获取失败";
			m_logFile->flush();
			m_db.FreeResult(res);
			break;
		}
		bodyRoot["message"] = "获取成功";
		bodyRoot["status"] = 1;

	} while (false);
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::ctrlToBectrlDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{

	Json::Value bodyRoot; // 返回的json数据
	Json::Value arrMain;

	// longitude精度  latitude纬度


	double ctrlLongitude[] = { 126.652713, 123.6548, 256.14568 };
	double ctrlLatitude[] = { 45.743479, 123.6548, 256.14568 };
	double beCtrlLongitude[] = { 122.124391 , 123.6548, 256.14568 };
	double beCtrlLatitude[] = { 37.519933 , 123.6548, 256.14568 };


	for (int i = 0; i < int(sizeof(ctrlLongitude) / sizeof(*ctrlLongitude)); i++) {
		Json::Value Item1;
		Json::Value Item2;
		Json::Value arrNotices;
		Item1[0] = ctrlLongitude[i];
		Item1[1] = ctrlLatitude[i];
		Item2[0] = beCtrlLongitude[i];
		Item2[1] = beCtrlLatitude[i];
		arrNotices["coords"].append(Item1);
		arrNotices["coords"].append(Item2);
		arrMain.append(arrNotices);
	}

	bodyRoot["list"] = arrMain;
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::updateUserInfoDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value bodyRoot; // 返回的json数据
	do {
		bodyRoot["message"] = "操作失败";
		bodyRoot["status"] = -2;

		auto it = parser.Headers().find("Authorization");
		if (it == parser.Headers().end()) {
			it = parser.Headers().find("authorization");
			if (it == parser.Headers().end()) {
				LOG_ERROR << "指定字段不存在（token）";
				m_logFile->flush();
				break;
			}
		}
		std::string phone = (*it).second;

		// 请求体数据
		Buffer buffer = parser.Body();
		// 解析请求体数据
		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			return -1;
		}

		std::string fields[] = { "avatar","name","password","sex","addr","phone" }; // body中对应的字段
		std::string db_fields[] = { "Uavatar","Uname","Upassword", "Usex", "Uaddr", "Uphone" }; // 数据库中对应的字段
		std::string fieldsValue[sizeof(fields) / sizeof(*fields)] = { "" };  // 表示要修改的字段值
		bool bupdate = false;   // 表示是否更新
		for (int i = 0; i < int(sizeof(fields) / sizeof(*fields)); i++) {
			if (true == root.isMember(fields[i])) {
				// 数据段存在
				if (!root[fields[i]].isArray()) {
					// 字段值不能为数组类型
					std::string val = root[fields[i]].asString();
					if (val != "") {
						fieldsValue[i] = val;
						bupdate = true;
					}
				}
				else {
					bupdate = false;
					break;
				}

			}
		}
		if (!bupdate) {
			break;
		}
		std::string sql(512, '\0');
		if (fieldsValue[5] != "") {
			// 电话号码要修改
			// 先判断它要修改的电话号码是否已经存在在数据库中
			snprintf((char*)sql.c_str(), sql.size(), "select Uid from ctuser where Uphone = '%s';\0\0", fieldsValue[5].c_str());
			LOG_INFO << "updateUserInfoDispose select user sql:[" << sql.c_str() << "]";
			m_logFile->flush();
			if (false == m_db.Execute(sql)) {
				LOG_ERROR << "sql 执行失败";
				m_logFile->flush();
				break;
			}
			const ResultRecord* res = m_db.GetResult();
			if (res->GetRecordsSize() > 0) {
				// 当前号码已经存在了
				m_db.FreeResult(res);
				bodyRoot["message"] = "修改的号码已经被注册";
				break;
			}
			m_db.FreeResult(res);
		}
		std::string updateSet;
		// 拼接update的set模块
		bupdate = false;
		for (size_t i = 0; i < (sizeof(fields) / sizeof(*fields)); i++) {
			if (fieldsValue[i] != "") {
				// 有这个字段要修改
				if (bupdate == true) {
					updateSet += ',';
				}
				else {
					bupdate = true;
				}
				updateSet += (db_fields[i] + " = \'" + fieldsValue[i] + "\'");
			}
		}

		// 首先获取到用户的id
		snprintf((char*)sql.c_str(), sql.size(), "select Uid from ctuser where Uphone = '%s';\0\0", phone.c_str());
		LOG_INFO << "updateUserInfoDispose select user sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		long uid = -1;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			uid = rec[0][0].asLONGLONG();*/

			uid = (*res)[0]["Uid"].asLONGLONG();

			m_db.FreeResult(res);
		}
		else {
			m_db.FreeResult(res);
			break;
		}
		// 保存用户头像

		if (true == root.isMember(fields[0])) {
			if (true == root[fields[0]].isString()) {

			}
		}

		// 修改数据库
		snprintf((char*)sql.c_str(), sql.size(), "update ctuser set %s where Uid = '%d';\0\0", updateSet.c_str(), uid);
		LOG_INFO << "updateUserInfoDispose update user sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		bodyRoot["message"] = "success";
		bodyRoot["status"] = 1;
	} while (false);
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::getNewNoticeDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value bodyRoot; // 返回的json数据
	do {
		bodyRoot["message"] = "暂无公告";
		bodyRoot["status"] = -2;

		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "SELECT content FROM notice ORDER BY create_time DESC LIMIT 1;\0\0");
		LOG_INFO << "getNewNoticeDispose select notice sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		std::string content;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			content = rec[0][0];*/

			content = (*res)[0]["content"];

			m_db.FreeResult(res);
		}
		else {
			m_db.FreeResult(res);
			break;
		}
		bodyRoot["message"] = "公告内容";
		bodyRoot["status"] = 1;
		bodyRoot["content"] = content.c_str();
	} while (false);
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::getAllNoticeDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value bodyRoot; // 返回的json数据
	do {
		bodyRoot["message"] = "获取失败";
		bodyRoot["status"] = -2;
		bodyRoot["total"] = 0;
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "SELECT id, title, content, create_time FROM notice;\0\0");
		LOG_INFO << "getAllNoticeDispose select notice sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Json::Value arrNotices;
		Json::Value Item;
		size_t recSize = res->GetRecordsSize();
		if (recSize > 0) {
			// Records rec = res->GetRecords();
			for (size_t i = 0; i < recSize; i++) {
				/*Item["id"] = (int)rec[i][0].asLONGLONG();
				Item["title"] = rec[i][1];
				Item["message"] = rec[i][2];
				Item["createdAt"] = rec[i][3];*/

				Item["id"] = (int)(*res)[i]["id"].asLONGLONG();
				Item["title"] = (*res)[i]["title"];
				Item["message"] = (*res)[i]["content"];
				Item["createdAt"] = (*res)[i]["create_time"];
				arrNotices.append(Item);
			}
			m_db.FreeResult(res);
		}
		else {
			m_db.FreeResult(res);
			break;
		}
		bodyRoot["total"] = recSize;
		bodyRoot["notice"] = arrNotices;
		bodyRoot["message"] = "获取成功";
		bodyRoot["status"] = 1;
	} while (false);
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::backLoginDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Buffer phone = url["phone"];       // 管理员手机号
	Buffer password = url["password"]; // 管理员密码
	LOG_INFO << "admin login: phone [" << phone.c_str() << "] password [" << password.c_str() << "]";
	m_logFile->flush();
	Json::Value body;
	body["message"] = "用户名或密码错误";
	body["status"] = 2;
	do {
		if ((phone == "") || (password == "")) {
			LOG_INFO << "没有数据的登录请求";
			m_logFile->flush();
			break;
		}
		// 数据库的查询
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "select password from manager where phone='%s';\0\0", phone.c_str());
		LOG_INFO << "backLoginDispose select password sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 执行sql查找密码
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Buffer upassword;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			upassword = rec[0][0];*/

			upassword = (*res)[0]["password"];

			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		LOG_INFO << "数据库" << phone << "用户密码:[" << upassword.c_str() << "]";
		m_logFile->flush();
		if (password == upassword) {
			// 验证成功
			body["message"] = "success";
			body["status"] = 0;
			break;
		}
	} while (false);
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backGetUserListDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	int page = (int)url["page"].asLONGLONG();
	int pageSize = (int)url["pageSize"].asLONGLONG();

	Json::Value body;
	body["message"] = "获取失败";
	body["status"] = 2;
	do {
		if ((page <= 0) || (pageSize <= 0)) {
			LOG_INFO << "数据有误的用户获取请求";
			m_logFile->flush();
			break;
		}
		// 先查询出用户的总数
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "select COUNT(Uid) as userCount from ctuser;\0\0");
		LOG_INFO << "backGetUserListDispose select user total number sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		int total = 0;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			total = (int)rec[0][0].asLONGLONG();*/

			total = (int)(*res)[0]["userCount"].asLONGLONG();

			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		LOG_INFO << "用户总数为：" << total;
		m_logFile->flush();
		// 判断当前的查询范围是否符合规范
		if ((page - 1) * (pageSize) >= total) {
			// 当前要查看的页，已经大于用户总数了
			LOG_ERROR << "分页数据太大: page=" << page << " pageSize=" << pageSize << " total=" << total;
			m_logFile->flush();
			break;
		}
		// 查找指定范围的数据
		// SELECT ctuser.Uid, Uname, Usex, Uaddr, Uphone, Uregister_time,`status`,grade,expire_time,Ubalance,acc_amount,ctrl_time,ctrl_num,surplus_num  FROM ctuser, vip WHERE ctuser.Uid=vip.Uid LIMIT 10, 10;
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"select ctuser.Uid, Uname, Usex, Uaddr, Uphone, Uregister_time, vip.`status`, grade, expire_time, Ubalance, acc_amount, ctrl_time, ctrl_num, surplus_num, Uforbidden, Uexpire from ctuser, vip WHERE ctuser.Uid = vip.Uid limit %d, %d;\0\0",
			(page - 1) * pageSize,
			pageSize
		);
		LOG_INFO << "backGetUserListDispose select user info sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Json::Value list;
		Json::Value arrUInfo; // 数组
		Json::Value eItem;  // 用户的每一项信息
		if (res->GetRecordsSize() > 0) {
			// 获取字段信息
			// FieldInfos fInfo = res->GetFieldInfos();
			/*for (auto it : fInfo) {
				LOG_ERROR << "字段信息：" << it.field_name;
			}*/

			// TODO:字段数组封装

			// Records rec = res->GetRecords();
			for (int i = 0; i < (int)res->GetRecordsSize(); i++) {
				/*eItem["userId"] = rec[i][0];
				eItem["username"] = rec[i][1];
				eItem["sex"] = (int)rec[i][2].asLONGLONG();
				eItem["address"] = rec[i][3];
				eItem["phone"] = rec[i][4];
				eItem["registerTime"] = rec[i][5];
				eItem["vipStatus"] = (int)rec[i][6].asLONGLONG();
				eItem["vipLevel"] = rec[i][7];
				eItem["expirationDate"] = rec[i][8];
				eItem["balance"] = rec[i][9];
				eItem["totalRecharge"] = rec[i][10];
				eItem["deviceControlTime"] = rec[i][11];
				eItem["controlCount"] = (int)rec[i][12].asLONGLONG();
				eItem["remainingCount"] = (int)rec[i][13].asLONGLONG();
				eItem["forbidden"] = (int)rec[i][14].asLONGLONG();
				eItem["expire"] = (int)rec[i][15].asLONGLONG();*/

				eItem["userId"] = (*res)[i]["Uid"];
				eItem["username"] = (*res)[i]["Uname"];
				eItem["sex"] = (int)(*res)[i]["Usex"].asLONGLONG();
				eItem["address"] = (*res)[i]["Uaddr"];
				eItem["phone"] = (*res)[i]["Uphone"];
				eItem["registerTime"] = (*res)[i]["Uregister_time"];
				eItem["vipStatus"] = (int)(*res)[i]["status"].asLONGLONG();
				eItem["vipLevel"] = (*res)[i]["grade"];
				eItem["expirationDate"] = (*res)[i]["expire_time"];
				eItem["balance"] = (*res)[i]["Ubalance"];
				eItem["totalRecharge"] = (*res)[i]["acc_amount"];
				eItem["deviceControlTime"] = (*res)[i]["ctrl_time"];
				eItem["controlCount"] = (int)(*res)[i]["ctrl_num"].asLONGLONG();
				eItem["remainingCount"] = (int)(*res)[i]["surplus_num"].asLONGLONG();
				eItem["forbidden"] = ((*res)[i]["Uforbidden"].asLONGLONG() == 1) ? true : false;  // 1表示启用
				eItem["expire"] = ((*res)[i]["Uexpire"].asLONGLONG() == 1) ? true : false;        // 0表示停用
				arrUInfo.append(eItem);
			}
			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		list["list"] = arrUInfo;
		list["total"] = (total / pageSize) + ((total % pageSize) > 0 ? 1 : 0);
		body["data"] = list;
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
	bodyData = body.toStyledString();
	return 0;
}



int CCrossTimeServer::backSearchUserListDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Buffer addr = url["address"];
	addr = CTools::DecodeURIComponent(addr).c_str(); // 地区查找有特殊符号（中文），进行decode转码

	Json::Value body;
	body["message"] = "获取失败";
	body["status"] = 2;
	do {
		if (addr == "") {
			LOG_INFO << "数据有误的用户获取请求";
			m_logFile->flush();
			break;
		}
		// 查找指定范围的数据
		// SELECT ctuser.Uid, Uname, Usex, Uaddr, Uphone, Uregister_time,`status`,grade,expire_time,Ubalance,acc_amount,ctrl_time,ctrl_num,surplus_num  FROM ctuser, vip WHERE ctuser.Uid=vip.Uid AND Uaddr LIKE '%北%';
		std::string sql(512, '\0');
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"select ctuser.Uid, Uname, Usex, Uaddr, Uphone, Uregister_time, vip.`status`, grade, expire_time, Ubalance, acc_amount, ctrl_time, ctrl_num, surplus_num, Uforbidden, Uexpire from ctuser, vip WHERE ctuser.Uid = vip.Uid and Uaddr like '%%%s%%';\0\0",
			addr.c_str()
		);
		LOG_INFO << "backSearchUserListDispose select user info sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Json::Value list;
		Json::Value arrUInfo; // 数组
		Json::Value eItem;  // 用户的每一项信息
		if (res->GetRecordsSize() > 0) {
			// Records rec = res->GetRecords();
			for (int i = 0; i < (int)res->GetRecordsSize(); i++) {
				/*eItem["userId"] = rec[i][0];
				eItem["username"] = rec[i][1];
				eItem["sex"] = (int)rec[i][2].asLONGLONG();
				eItem["address"] = rec[i][3];
				eItem["phone"] = rec[i][4];
				eItem["registerTime"] = rec[i][5];
				eItem["vipStatus"] = (int)rec[i][6].asLONGLONG();
				eItem["vipLevel"] = rec[i][7];
				eItem["expirationDate"] = rec[i][8];
				eItem["balance"] = rec[i][9];
				eItem["totalRecharge"] = rec[i][10];
				eItem["deviceControlTime"] = rec[i][11];
				eItem["controlCount"] = (int)rec[i][12].asLONGLONG();
				eItem["remainingCount"] = (int)rec[i][13].asLONGLONG();*/


				eItem["userId"] = (*res)[i]["Uid"];
				eItem["username"] = (*res)[i]["Uname"];
				eItem["sex"] = (int)(*res)[i]["Usex"].asLONGLONG();
				eItem["address"] = (*res)[i]["Uaddr"];
				eItem["phone"] = (*res)[i]["Uphone"];
				eItem["registerTime"] = (*res)[i]["Uregister_time"];
				eItem["vipStatus"] = (int)(*res)[i]["status"].asLONGLONG();
				eItem["vipLevel"] = (*res)[i]["grade"];
				eItem["expirationDate"] = (*res)[i]["expire_time"];
				eItem["balance"] = (*res)[i]["Ubalance"];
				eItem["totalRecharge"] = (*res)[i]["acc_amount"];
				eItem["deviceControlTime"] = (*res)[i]["ctrl_time"];
				eItem["controlCount"] = (int)(*res)[i]["ctrl_num"].asLONGLONG();
				eItem["remainingCount"] = (int)(*res)[i]["surplus_num"].asLONGLONG();
				eItem["forbidden"] = (int)(*res)[i]["Uforbidden"].asLONGLONG();
				eItem["expire"] = (int)(*res)[i]["Uexpire"].asLONGLONG();


				arrUInfo.append(eItem);
			}
			// total = (int)rec[0][0].asLONGLONG();
			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		list["list"] = arrUInfo;
		body["data"] = list;
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backOperateUserDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{

	Json::Value body;
	body["message"] = "操作失败";
	body["status"] = 2;

	do {
		// 获取请求数据
		Buffer buffer = parser.Body();
		// 解析请求数据，json格式
		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			break;
		}
		std::string fields[] = { "userIds", "type" };  // 数据中应该存在的字段
		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
			break;
		}
		for (int i = 0; i<int(sizeof(fields) / sizeof(*fields)); ++i) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				goto BREAK;
			}
			if (!root[fields[i]].isArray() && !root[fields[i]].isString()) {
				// 字段类型不满足要求
				goto BREAK;
			}
		}
		// JSON数组的解析
		std::string operation = root[fields[1]].asString().c_str();
		if (root[fields[0]].isArray() == false) {
			LOG_ERROR << "用户id不是数组类型";
			m_logFile->flush();
			break;
		}
		Json::Value idArr = root[fields[0]];
		idArr.size();

		/*UPDATE users
			SET status = 'active'
			WHERE user_id IN (1, 2, 3);*/

		std::string subSql;  // 子sql语句
		for (int i = 0; i < (int)idArr.size() - 1; ++i) {
			subSql += idArr[i].asString().c_str();
			subSql += ',';
		}
		subSql += idArr[idArr.size() - 1].asString().c_str();

		std::string sql; // sql语句
		if (operation == "delete") { // 删除
			// UPDATE ctuser SET Uexpire = 1 WHERE Uid IN (12,15);
			sql = "UPDATE ctuser SET Uexpire = 1 WHERE Uid IN (" + subSql + ");\0";
		}
		else if (operation == "disabled") { // 禁用
			// UPDATE ctuser SET Uforbidden = 1 WHERE Uid IN (12,15);
			sql = "UPDATE ctuser SET Uforbidden = 1 WHERE Uid IN (" + subSql + ");\0";
		}
		else if (operation == "enabled") { // 启用
			// UPDATE ctuser SET Uforbidden = 0 WHERE Uid IN (12,15);
			sql = "UPDATE ctuser SET Uforbidden = 0 WHERE Uid IN (" + subSql + ");";
		}
		LOG_INFO << "backOperateUserDispose update user info:[" << sql << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
BREAK:
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backDeleteUserDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{

	Buffer uId = url["userId"];

	Json::Value body;
	body["message"] = "执行失败";
	body["status"] = 2;
	do {
		/*std::basic_string<char>::size_type index = uId.find("user-", 0);
		if (index == std::string::npos) {
			break;
		}
		std::string id = uId.substr(index + 5, uId.size() - index - 5);*/
		std::string id = uId.c_str();
		LOG_INFO << "backDeleteUserDispose delete user id:" << id;
		m_logFile->flush();
		// 检测是否都是数字
		for (int i = 0; i < (int)id.size(); i++) {
			if (id[i] < '0' || id[i] > '9') {
				// 这个表示不是数字
				goto BREAK;
			}
		}
		// DELETE FROM ctuser where Uid=13;
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "delete from ctuser where Uid=%s;\0\0", id.c_str());
		LOG_INFO << "backDeleteUserDispose delete user info sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
BREAK:
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backDeleteDevicesDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value body;
	body["message"] = "操作失败";
	body["status"] = 2;
	do {
		// 请求体数据
		Buffer buffer = parser.Body();
		// 解析请求体数据
		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			return -1;
		}
		std::string fields[] = { "devicesIds" }; // body中对应的字段
		for (int i = 0; i<int(sizeof(fields) / sizeof(*fields)); i++) {
			if (true == root.isMember(fields[i])) { // 数据段存在
				if (!root[fields[i]].isArray()) {
					// 字段值不是数组类型
					goto BREAK;
				}
			}
		}
		Json::Value idArr = root[fields[0]];

		std::string subSql;
		for (int i = 0; i < (int)idArr.size() - 1; ++i) {
			subSql += idArr[i].asString().c_str();
			subSql += ',';
		}
		subSql += idArr[idArr.size() - 1].asString().c_str();

		std::string sql;
		sql = "DELETE FROM equipment WHERE Eid IN (" + subSql + ");";
		LOG_INFO << "backDeleteDevicesDispose delete device sql:[" << sql.c_str() << "]";
		m_logFile->flush();

		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
BREAK:
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backGetDevicesListDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	int page = (int)url["page"].asLONGLONG();
	int pageSize = (int)url["pageSize"].asLONGLONG();

	Json::Value body;
	body["message"] = "获取失败";
	body["status"] = 2;
	do {
		if ((page <= 0) || (pageSize <= 0)) {
			LOG_INFO << "数据有误的设备获取请求";
			m_logFile->flush();
			break;
		}
		// 先获取出设备的总数
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "select COUNT(Eid) as eCount from equipment WHERE app_status='1';\0\0");
		LOG_INFO << "backGetDevicesListDispose select user total number sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		int total = 0;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			total = (int)rec[0][0].asLONGLONG();*/
			total = (int)(*res)[0]["eCount"].asLONGLONG();
			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		LOG_INFO << "设备总数为：" << total;
		m_logFile->flush();
		// 判断当前的查询范围是否符合规范
		if ((page - 1) * (pageSize) >= total) {
			// 当前要查看的页，已经大于用户总数了
			LOG_ERROR << "分页数据太大: page=" << page << " pageSize=" << pageSize << " total=" << total;
			m_logFile->flush();
			break;
		}
		// 查找指定范围的数据
		// SELECT Eid, Ename, Uid, secret_key, app_time, app_status FROM equipment LIMIT 0,10;
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"SELECT Eid, Ename, Uid, secret_key, app_time FROM equipment WHERE app_status='1' LIMIT %d, %d;\0\0",
			(page - 1) * pageSize,
			pageSize
		);
		LOG_INFO << "backGetDevicesListDispose select equipment info sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Json::Value list;
		Json::Value arrUInfo; // 数组
		Json::Value eItem;  // 用户的每一项信息
		if (res->GetRecordsSize() > 0) {
			// Records rec = res->GetRecords();
			for (int i = 0; i < (int)res->GetRecordsSize(); i++) {
				/*eItem["devicesId"] = (int)rec[i][0].asLONGLONG();
				eItem["devicesName"] = rec[i][1];
				eItem["userId"] = (int)rec[i][2].asLONGLONG();
				eItem["secretKey"] = rec[i][3];
				eItem["applyTime"] = rec[i][4];*/


				eItem["devicesId"] = (int)(*res)[i]["Eid"].asLONGLONG();
				eItem["devicesName"] = (*res)[i]["Ename"];
				eItem["userId"] = (int)(*res)[i]["Uid"].asLONGLONG();
				eItem["secretKey"] = (*res)[i]["secret_key"];
				eItem["applyTime"] = (*res)[i]["app_time"];

				arrUInfo.append(eItem);
			}
			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		list["list"] = arrUInfo;
		list["total"] = (total / pageSize) + ((total % pageSize) > 0 ? 1 : 0);
		body["data"] = list;
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backSearchDevicesListDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Buffer uId = url["userId"];

	Json::Value body;
	body["message"] = "执行失败";
	body["status"] = 2;
	do {
		/*std::basic_string<char>::size_type index = uId.find("user-", 0);
		if (index == std::string::npos) {
			break;
		}
		std::string id = uId.substr(index + 5, uId.size() - index - 5);*/
		std::string id = uId.c_str();
		LOG_INFO << "backDeleteUserDispose delete user id:" << id;
		m_logFile->flush();
		// 检测是否都是数字
		for (int i = 0; i < (int)id.size(); i++) {
			if (id[i] < '0' || id[i] > '9') {
				// 这个表示不是数字
				goto BREAK;
			}
		}
		// 查找属于用户的所有设备
		// SELECT Eid, Ename, Uid, secret_key, app_time, app_status FROM equipment WHERE Uid=1;
		std::string sql(512, '\0');
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"SELECT Eid, Ename, Uid, secret_key, app_time FROM equipment WHERE Uid=%s and app_status='1';\0\0",
			id.c_str()
		);
		LOG_INFO << "backSearchDevicesListDispose select user equipment info sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Json::Value list;
		Json::Value arrUInfo; // 数组
		Json::Value eItem;  // 用户的每一项信息
		if (res->GetRecordsSize() > 0) {
			// Records rec = res->GetRecords();
			for (int i = 0; i < (int)res->GetRecordsSize(); i++) {
				/*eItem["devicesId"] = (int)rec[i][0].asLONGLONG();
				eItem["devicesName"] = rec[i][1];
				eItem["userId"] = (int)rec[i][2].asLONGLONG();
				eItem["secretKey"] = rec[i][3];
				eItem["applyTime"] = rec[i][4];*/


				eItem["devicesId"] = (int)(*res)[i]["Eid"].asLONGLONG();
				eItem["devicesName"] = (*res)[i]["Ename"];
				eItem["userId"] = (int)(*res)[i]["Uid"].asLONGLONG();
				eItem["secretKey"] = (*res)[i]["secret_key"];
				eItem["applyTime"] = (*res)[i]["app_time"];

				arrUInfo.append(eItem);
			}
			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		list["list"] = arrUInfo;
		body["data"] = list;
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
BREAK:
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backGetApplyDevicesDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value body;
	body["message"] = "获取失败";
	body["status"] = 2;
	do {
		// 先获取出设备的总数
		std::string sql(512, '\0');
		// 查找未处理的申请设备
		// SELECT Eid, Ename, Uid, secret_key, app_time FROM equipment WHERE app_status='0';
		snprintf((char*)sql.c_str(), sql.size(), "SELECT Eid, Ename, Uid, secret_key, app_time FROM equipment WHERE app_status='0'\0\0");
		LOG_INFO << "backGetApplyDevicesDispose select equipment info sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Json::Value list;
		Json::Value arrUInfo; // 数组
		Json::Value eItem;  // 用户的每一项信息
		if (res->GetRecordsSize() > 0) {
			// Records rec = res->GetRecords();
			for (int i = 0; i < (int)res->GetRecordsSize(); i++) {
				/*eItem["devicesId"] = (int)rec[i][0].asLONGLONG();
				eItem["devicesName"] = rec[i][1];
				eItem["userId"] = (int)rec[i][2].asLONGLONG();
				eItem["secretKey"] = rec[i][3];
				eItem["applyTime"] = rec[i][4];*/



				eItem["devicesId"] = (int)(*res)[i]["Eid"].asLONGLONG();
				eItem["devicesName"] = (*res)[i]["Ename"];
				eItem["userId"] = (int)(*res)[i]["Uid"].asLONGLONG();
				eItem["secretKey"] = (*res)[i]["secret_key"];
				eItem["applyTime"] = (*res)[i]["app_time"];



				arrUInfo.append(eItem);
			}
			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		list["list"] = arrUInfo;
		body["data"] = list;
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backApplyDevicesDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{

	Json::Value body; // 返回的json数据
	body["message"] = "操作失败";
	body["status"] = 2;
	do {
		// 获取请求数据
		Buffer buffer = parser.Body();
		// 解析请求数据，json格式
		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			break;
		}
		std::string fields[] = { "devicesId","type" };  // 数据中应该存在的字段
		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
			break;
		}
		for (int i = 0; i<int(sizeof(fields) / sizeof(*fields)); ++i) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				goto BREAK;
			}
			if (!root[fields[i]].isArray() && !root[fields[i]].isString()) {
				// 字段类型不满足要求
				goto BREAK;
			}
		}
		// JSON数据的解析
		std::string operation = root[fields[1]].asString().c_str();
		if (root[fields[0]].isArray() == false) {
			LOG_ERROR << "数据格式不符合要求，需要为数组类型 body:[" << buffer << "]";
			m_logFile->flush();
		}
		Json::Value idArr = root[fields[0]];

		std::string subSql, sql;
		for (int i = 0; i < (int)idArr.size() - 1; ++i) {
			subSql += idArr[i].asString().c_str();
			subSql += ',';
		}
		subSql += idArr[idArr.size() - 1].asString().c_str();

		if (operation == "approve") {
			// 同意把0变1  UPDATE equipment SET app_status = 1 WHERE Eid IN (12,15);
			sql = "UPDATE equipment SET app_status = 1 WHERE Eid IN (" + subSql + ");";
		}
		else if (operation == "reject") {
			// 拒绝直接删除  DELETE FROM equipment WHERE Eid IN (12,15);
			sql = "DELETE FROM equipment WHERE Eid IN (" + subSql + ");";
		}
		LOG_INFO << "backApplyDevicesDispose operation equipment info sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
BREAK:
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backIssueNoticeDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value bodyRoot; // 返回的json数据
	bodyRoot["message"] = "操作失败";
	bodyRoot["status"] = 2;
	do {
		// 请求体数据
		Buffer buffer = parser.Body();
		// 解析请求体数据
		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(buffer, root)) {
			// body解析失败
			LOG_ERROR << "json 数据解析失败";
			m_logFile->flush();
			break;
		}
		std::string fields[] = { "phone","title","content","createTime" };
		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
			LOG_ERROR << "数据字段个数不匹配";
			m_logFile->flush();
			break;
		}
		for (int i = 0; i<int(sizeof(fields) / sizeof(*fields)); i++) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				LOG_ERROR << "要求的字段不存在";
				m_logFile->flush();
				goto BREAK;
			}
			else if ((false == root[fields[i]].isString())) {
				// 数据不能为数组类型
				LOG_ERROR << "数据类型不匹配";
				m_logFile->flush();
				goto BREAK;
			}
		}
		std::string sql(512, '\0');
		// 先通过手机号查询管理员id
		// SELECT id FROM manager WHERE phone='15046543174';
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"SELECT id FROM manager WHERE phone='%s';\0\0",
			root[fields[0]].asString().c_str()
		);
		LOG_INFO << "backIssueNoticeDispose select admin id sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 向数据库中插入数据
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		std::string adminId;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			adminId = rec[0][0];*/

			adminId = (*res)[0]["id"];

			m_db.FreeResult(res);
		}
		else {
			m_db.FreeResult(res);
			LOG_INFO << "没有数据";
			m_logFile->flush();
			break;
		}


		// INSERT INTO notice(mid, title, content,create_time,`update`) VALUES(1,'你好','你好','2024-05-10 16:38:16','2024-05-10 16:38:16');
		snprintf((char*)sql.c_str(), sql.size(), "INSERT INTO notice(mid, title, content,create_time,`update`) VALUES(%s,'%s','%s','%s','%s');\0\0", adminId.c_str(), root[fields[1]].asString().c_str(), root[fields[2]].asString().c_str(), root[fields[3]].asString().c_str(), root[fields[3]].asString().c_str());
		LOG_INFO << "backIssueNoticeDispose notice user sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 向数据库中插入数据
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		bodyRoot["message"] = "success";
		bodyRoot["status"] = 1;
	} while (false);
BREAK:
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::backGetNoticeListDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{

	int page = (int)url["page"].asLONGLONG();
	int pageSize = (int)url["pageSize"].asLONGLONG();

	Json::Value body;
	body["message"] = "获取失败";
	body["status"] = 2;

	do {
		if ((page <= 0) || (pageSize <= 0)) {
			LOG_INFO << "数据有误的通告获取请求";
			m_logFile->flush();
			break;
		}
		// 先获取出设备的总数
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "select COUNT(id) as nCount from notice;\0\0");
		LOG_INFO << "backGetNoticeListDispose select notice total number sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		const ResultRecord* res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		int total = 0;
		if (res->GetRecordsSize() > 0) {
			/*Records rec = res->GetRecords();
			total = (int)rec[0][0].asLONGLONG();*/

			total = (int)(*res)[0]["nCount"].asLONGLONG();

			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		LOG_INFO << "公告总数为：" << total;
		m_logFile->flush();
		// 判断当前的查询范围是否符合规范
		if ((page - 1) * (pageSize) >= total) {
			// 当前要查看的页，已经大于通知总数了
			LOG_ERROR << "分页数据太大: page=" << page << " pageSize=" << pageSize << " total=" << total;
			m_logFile->flush();
			break;
		}
		// 查找指定范围的数据
		// SELECT id,mid,title,content,create_time,`update` FROM notice LIMIT 0,10;
		snprintf(
			(char*)sql.c_str(),
			sql.size(),
			"SELECT id, mid, title, content, create_time, `update` FROM notice LIMIT %d, %d;\0\0",
			(page - 1) * pageSize,
			pageSize
		);
		LOG_INFO << "backGetNoticeListDispose select notice info sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		res = m_db.GetResult();
		if (res == NULL) {
			break;
		}
		Json::Value list;
		Json::Value arrUInfo; // 数组
		Json::Value eItem;  // 用户的每一项信息
		if (res->GetRecordsSize() > 0) {
			// Records rec = res->GetRecords();
			for (int i = 0; i < (int)res->GetRecordsSize(); i++) {
				/*eItem["noticeId"] = (int)rec[i][0].asLONGLONG();
				eItem["admId"] = (int)rec[i][1].asLONGLONG();
				eItem["title"] = rec[i][2];
				eItem["content"] = rec[i][3];
				eItem["createTime"] = rec[i][4];
				eItem["updateTime"] = rec[i][5];*/



				eItem["noticeId"] = (int)(*res)[i]["id"].asLONGLONG();
				eItem["admId"] = (int)(*res)[i]["mid"].asLONGLONG();
				eItem["title"] = (*res)[i]["title"];
				eItem["content"] = (*res)[i]["content"];
				eItem["createTime"] = (*res)[i]["create_time"];
				eItem["updateTime"] = (*res)[i]["update"];


				arrUInfo.append(eItem);
			}
			m_db.FreeResult(res);
		}
		else m_db.FreeResult(res);
		list["list"] = arrUInfo;
		list["total"] = (total / pageSize) + ((total % pageSize) > 0 ? 1 : 0);
		body["data"] = list;
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
	bodyData = body.toStyledString();
	return 0;
}

int CCrossTimeServer::backUpdateNoticeDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value bodyRoot; // 返回的json数据
	bodyRoot["message"] = "操作失败";
	bodyRoot["status"] = 2;
	do {
		// 请求体数据
		Buffer buffer = parser.Body();
		// 解析请求体数据
		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(buffer, root)) {
			// body解析失败
			break;
		}

		std::string fields[] = { "noticeId", "title","content","updateTime" }; // body中对应的字段
		std::string db_fields[] = { "id", "title", "content","update" }; // 数据库中对应的字段
		std::string fieldsValue[sizeof(fields) / sizeof(*fields)] = { "" }; // 表示要修改的字段值
		bool bUpdate = false; // 表示是否更新
		for (int i = 0; i<int(sizeof(fields) / sizeof(*fields)); i++) {
			if (true == root.isMember(fields[i])) {
				// 数据段存在
				if (!root[fields[i]].isArray()) {
					// 字段值不能为数组类型
					std::string val = root[fields[i]].asString();
					if (val != "") {
						fieldsValue[i] = val;
						bUpdate = true;
					}
				}
				else {
					bUpdate = false;
					break;
				}
			}
		}
		if (!bUpdate) {
			break;
		}
		// id和time不能为空
		if (fieldsValue[0] == "" || fieldsValue[sizeof(fieldsValue) / sizeof(*fieldsValue) - 1] == "") {
			// 必须数据不存在
			break;
		}
		std::string updateSet; // 要更新的字段
		// 拼接update的set模块
		bUpdate = false;
		for (size_t i = 1; i < (sizeof(fields) / sizeof(*fields) - 1); i++) {
			if (fieldsValue[i] != "") {
				// 有这个字段要修改
				if (bUpdate == true) {
					updateSet += ',';
				}
				else {
					bUpdate = true;
				}
				updateSet += (db_fields[i] + " = \'" + fieldsValue[i] + "\'");
			}
		}
		if (updateSet.size() <= 0) {
			// 没有东西要更新
			break;
		}
		// 修改数据库
		std::string sql(512, '\0');
		// UPDATE notice SET title='通知',content='通知', `update`='2024-05-10 18:30:30' WHERE id=2;
		snprintf((char*)sql.c_str(), sql.size(), "UPDATE notice SET %s,`update`='%s' WHERE id=%s;\0\0", updateSet.c_str(), fieldsValue[sizeof(fieldsValue) / sizeof(*fieldsValue) - 1].c_str(), fieldsValue[0].c_str());
		LOG_INFO << "backUpdateNoticeDispose update notice sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		bodyRoot["message"] = "success";
		bodyRoot["status"] = 1;
	} while (false);
	bodyData = bodyRoot.toStyledString();
	return 0;
}

int CCrossTimeServer::backDeleteNoticeDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	Json::Value body;
	body["message"] = "操作失败";
	body["status"] = 2;

	do {
		// 获取请求数据
		Buffer buffer = parser.Body();
		// 解析请求数据，json格式
		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(buffer, root)) {
			// 解析失败
			break;
		}
		std::string fields[] = { "noticeIds" };
		if (root.getMemberNames().size() < (sizeof(fields) / sizeof(*fields))) {
			break;
		}
		for (int i = 0; i<int(sizeof(fields) / sizeof(*fields)); ++i) {
			if (false == root.isMember(fields[i])) {
				// 数据不匹配
				LOG_INFO << "数据不匹配 body:[" << buffer << "]";
				m_logFile->flush();
				goto BREAK;
			}
			if (!root[fields[i]].isArray() && !root[fields[i]].isString()) {
				// 字段类型不满足要求
				LOG_INFO << "字段类型不满足要求 body:[" << buffer << "]";
				m_logFile->flush();
				goto BREAK;
			}
		}

		Json::Value idArr = root[fields[0]];
		std::string subSql;
		for (int i = 0; i < (int)idArr.size() - 1; ++i) {
			subSql += idArr[i].asString().c_str();
			subSql += ',';
		}
		subSql += idArr[idArr.size() - 1].asString().c_str();

		std::string sql;
		// DELETE FROM notice where id=3;
		sql = "DELETE FROM notice where id IN(" + subSql + ");";
		LOG_INFO << "backDeleteNoticeDispose delete notice info sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) {
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		body["message"] = "success";
		body["status"] = 1;
	} while (false);
BREAK:
	bodyData = body.toStyledString();
	return 0;
}

Buffer CCrossTimeServer::MakeAckPacket(std::string& bodyData)
{

	Buffer result = "HTTP/1.1 200 OK\r\n";
	time_t t;
	time(&t);
	tm* ptm = localtime(&t);
	char temp[64] = "";
	strftime(temp, sizeof(temp), "%a, %d %b %G %T GMT\r\n", ptm);
	Buffer Date = "Date: ";
	Date += temp;
	result += Date;

	//Content-Type: text/html; charset=utf-8\r\n
	result += "Server: CrossTimeServer/1.0\r\nContent-Type: text/html; charset=utf-8\r\n";

	// 解决跨域问题
	result += "Access-Control-Allow-Origin: *\r\n";
	result += "Access-Control-Allow-Methods: POST,GET,OPTIONS,DELETE\r\n";
	result += "Access-Control-Max-Age: 3600\r\n";
	result += "Access-Control-Allow-Headers: *\r\n";
	result += "Access-Control-Allow-Credentials: true\r\n";
	result += "Connection: keep-alive\r\n";
	result += "Keep-Alive: timeout=60\r\n";
	result += "Cache-Control: no-cache\r\n";


	if (bodyData.size() > 0) {
		// 有body数据
		memset(temp, 0, sizeof(temp));
		snprintf(temp, sizeof(temp), "%d", bodyData.size());
		result += (Buffer("Content-Length: ") + temp + "\r\n");
	}
	result += "X-Content-Type-Options: nosniff\r\nReferrer-Policy: same-origin\r\n\r\n";
	if (bodyData.size() > 0) {
		result += bodyData;
	}
	return result;
}

SOCKET CCrossTimeServer::maxFind()
{
	auto it = m_fds.begin();
	SOCKET max = *it;
	it++;
	for (; it != m_fds.end(); it++) {
		if (max < *it) {
			max = *it;
		}
	}
	return max;
}

void CCrossTimeServer::initLogger()
{
	Logger::setLogLevel(Logger::INFO);
	char name[256] = "./loggers/CTSLogger";
	m_logFile.reset(new LogFile(name, 200 * 1000));
	Logger::setOutput(outputFunc);
	Logger::setFlush(flushFunc);
}

void CCrossTimeServer::initFuncMapping()
{
	struct dis2func
	{
		std::string dis;
		DIS_FUNC    func;
	};
	// GET请求映射
	dis2func GETfd[] = {
		{ "bectrlOnline",           &CCrossTimeServer::bectrlOnlineDispose },
		{ "bectrlDownline",         &CCrossTimeServer::bectrlDownlineDispose },
		{ "getAvatarList",          &CCrossTimeServer::getAvatarListDispose },
		{ "equipmentlist",          &CCrossTimeServer::equipmentlistDispose },
		{ "getUserInfo",            &CCrossTimeServer::getUserInfoDispose },
		{ "getNewNotice",           &CCrossTimeServer::getNewNoticeDispose },
		{ "getAllNotice",           &CCrossTimeServer::getAllNoticeDispose },
		{ "getWeather",             &CCrossTimeServer::getWeatherDispose },
		{ "dataSummary",            &CCrossTimeServer::dataSummaryDispose },
		{ "currentOnline",          &CCrossTimeServer::currentOnlineDispose },
		{ "getEquipAll",            &CCrossTimeServer::getEquipAllDispose },
		{ "ctrlTobeCtrl",           &CCrossTimeServer::ctrlToBectrlDispose },
		{ "back/login",             &CCrossTimeServer::backLoginDispose },
		{ "back/getUserList",       &CCrossTimeServer::backGetUserListDispose },
		{ "back/searchUserList",    &CCrossTimeServer::backSearchUserListDispose },
		{ "back/searchDevicesList", &CCrossTimeServer::backSearchDevicesListDispose },
		{ "back/getDevicesList",    &CCrossTimeServer::backGetDevicesListDispose },
		{ "back/getNoticeList",     &CCrossTimeServer::backGetNoticeListDispose },
		{ "back/getApplyDevices",   &CCrossTimeServer::backGetApplyDevicesDispose }
	};
	// POST请求映射
	dis2func POSTfd[] = {
		{ "login",                  &CCrossTimeServer::loginDispose },
		{ "register",               &CCrossTimeServer::registerDispose },
		{ "test",                   &CCrossTimeServer::testDispose},
		{ "forgetPassword",         &CCrossTimeServer::forgetPasswordDispose },
		{ "verifyAnswer",           &CCrossTimeServer::verifyAnswerDispose },
		{ "updateUserInfo",         &CCrossTimeServer::updateUserInfoDispose },
		{ "startEquipmentCtrl",     &CCrossTimeServer::startEquipmentCtrlDispose },
		{ "endEquipmentCtrl",       &CCrossTimeServer::endEquipmentCtrlDispose },
		{ "ctrlEvent",              &CCrossTimeServer::ctrlEventDispose },
		{ "controlEquip",           &CCrossTimeServer::controlEquipDispose },
		{ "back/issueNotice",       &CCrossTimeServer::backIssueNoticeDispose },
		{ "back/operateUser",       &CCrossTimeServer::backOperateUserDispose },
		{ "back/applyDevices",      &CCrossTimeServer::backApplyDevicesDispose },
		{ "back/updateNotice",      &CCrossTimeServer::backUpdateNoticeDispose }
	};
	// DELETE请求映射
	dis2func DELETEfd[] = {
		{ "back/deleteUser",        &CCrossTimeServer::backDeleteUserDispose },
		{ "back/deleteNotice",      &CCrossTimeServer::backDeleteNoticeDispose },
		{ "back/deleteDevices",     &CCrossTimeServer::backDeleteDevicesDispose }
	};
	// GET请求注册
	for (size_t i = 0; i < sizeof(GETfd) / sizeof(*GETfd); i++) {
		m_mGETUriDisfunc.insert(std::pair<std::string, DIS_FUNC>(GETfd[i].dis, GETfd[i].func));
	}
	// POST请求注册
	for (size_t i = 0; i < sizeof(POSTfd) / sizeof(*POSTfd); i++) {
		m_mPOSTUriDisfunc.insert(std::pair<std::string, DIS_FUNC>(POSTfd[i].dis, POSTfd[i].func));
	}
	// DELETE请求注册
	for (size_t i = 0; i < sizeof(DELETEfd) / sizeof(*DELETEfd); i++) {
		m_mDELETEUriDisfunc.insert(std::pair<std::string, DIS_FUNC>(DELETEfd[i].dis, DELETEfd[i].func));
	}
}

void CCrossTimeServer::outputFunc(const char* msg, int len)
{
	m_logFile->append(msg, len);
}

void CCrossTimeServer::flushFunc()
{
	m_logFile->flush();
}

std::map<SOCKET, clientInfo>::key_type CCrossTimeServer::findEInfo(std::string Ename)
{
	for (auto it = m_mapSockAddr.begin(); it != m_mapSockAddr.end(); it++) {
		if (it->second.isEquip == true) {
			if (it->second.eInfo.equipName == Ename) {
				return it->first;
			}
		}
	}
	return -1;
}
