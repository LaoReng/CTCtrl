#include "CrossTimeServer.h"
#include "Packet.h"
#include "DBHelper.h"

std::unique_ptr<LogFile> CCrossTimeServer::m_logFile;

CCrossTimeServer::CCrossTimeServer(const std::string& ip, unsigned short port)
	: m_serverSocket(INVALID_SOCKET)
	, m_maxFd(INVALID_SOCKET)
	, m_bServerInvalid(true)
{
	// 日志库初始化
	initLogger();

	if (false == initDatabase()) {
		LOG_ERROR << "数据库初始化失败";
		m_logFile->flush();
		exit(0);
	}
	memset(&m_serverAddr, 0, sizeof(m_serverAddr));
	FD_ZERO(&m_rdset); // 使用给的宏清空
	// memset(&m_rdset, 0, sizeof(m_rdset));
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(port);
	m_serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	if (SelectInit() == false) {
		LOG_ERROR << "服务器初始化失败";
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
	int optval = 1;
	if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
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
					FD_CLR(*it, &m_rdset);
					close(*it);
					if (m_mapSockAddr.find(*it) != m_mapSockAddr.end()) {
						m_mapSockAddr.erase(*it);  // 删除映射表中的客户端数据
					}
					m_fds.erase(it++);  // 删除数组中的客户端数据
					it--;
					break;
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

#define BUFFER_SIZE 1024000



int CCrossTimeServer::Dispose(SOCKET clientSocket)
{
	// HTTP协议解析
	char buf[BUFFER_SIZE] = "";
	ssize_t len = read(clientSocket, buf, sizeof(buf));
	LOG_INFO << "read len=" << len << "buffer:[" << buf << "]";
	m_logFile->flush();
	if (len <= 0) {
		// 客户端主动断开连接
		return -1;
	}
	std::string bodyData; // 应答包中的body数据部分
	int ret = HttpParser(buf, bodyData, clientSocket);
	if (ret < 0) {
		LOG_WARN << "http数据解析失败";
		m_logFile->flush();
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
	size_t size = parser.Parser(data);
	if (size == 0 || (parser.Errno() != 0)) {
		LOG_ERROR << "size=" << size << " http errno=" << parser.Errno();
		m_logFile->flush();
		return -1;
	}
	UrlParser url("https://81.70.91.154" + parser.Url());
	int ret = url.Parser();
	if (ret != 0) {
		LOG_ERROR << "ret=" << ret << " url:[" << ("https://81.70.91.154" + parser.Url()) << "]";
		m_logFile->flush();
		return -2;
	}

	if (parser.Method() == HTTP_OPTIONS) {
		LOG_INFO << "HTTP_OPTIONS uri:[" << url.Uri() << "]";
		m_logFile->flush();
		return 0;
	}
	if (parser.Method() == HTTP_GET) {
		// get处理
		Buffer uri = url.Uri();
		LOG_INFO << "HTTP_GET uri:[" << uri.c_str() << "]";
		m_logFile->flush();
		if (uri == "login") { // 登录
			return loginDispose(url, parser, bodyData);
		}
		if (uri == "bectrlOnline") { // 被控端上线
			return bectrlOnlineDispose(url, parser, bodyData, clientSocket);
		}
		if (uri == "equipmentlist") { // 获取设备列表
			return equipmentlistDispose(url, parser, bodyData);
		}
		if (uri == "getUserInfo") { // 获取个人信息
			return getUserInfoDispose(url, parser, bodyData);
		}
		if (uri == "getNewNotice") { // 获取最新公告
			return getNewNoticeDispose(url, parser, bodyData);
		}
		if (uri == "getAllNotice") { // 获取所有公告
			return getAllNoticeDispose(url, parser, bodyData);
		}
	}
	else if (parser.Method() == HTTP_POST) {
		// post 处理
		Buffer uri = url.Uri();
		LOG_INFO << "HTTP_POST uri:[" << uri.c_str() << "]";
		m_logFile->flush();
		if (uri == "register") { // 注册
			return registerDispose(url, parser, bodyData);
		}
		if (uri == "startEquipmentCtrl") { // 开始设备监控
			return startEquipmentCtrlDispose(url, parser, bodyData);
		}
		if (uri == "endEquipmentCtrl") { // 停止设备监控
			return endEquipmentCtrlDispose(url, parser, bodyData);
		}
		if (uri == "ctrlEvent") { // 鼠标键盘事件
			return ctrlEventDispose(url, parser, bodyData);
		}
		if (uri == "forgetPassword") { // 忘记密码
			return forgetPasswordDispose(url, parser, bodyData);
		}
		if (uri == "verifyAnswer") { // 验证答案
			return verifyAnswerDispose(url, parser, bodyData);
		}
		if (uri == "updateUserInfo") { // 修改用户信息
			return updateUserInfoDispose(url, parser, bodyData);
		}
	}
	return -7;
}

bool CCrossTimeServer::initDatabase()
{
	// CTCtrl666
	bool ret = m_db.Connect("root", "CTCtrl666", "");
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
	// 创建设备表
	sql = "CREATE TABLE IF NOT EXISTS `CrossTime`.`equipment` (\
		`Eid` int(11) NOT NULL AUTO_INCREMENT COMMENT '设备id',\
		`Ename` varchar(40) CHARACTER SET utf8 NOT NULL COMMENT '设备名',\
		`Uid` int(11) NOT NULL COMMENT '所属用户id',\
		`secret_key` varchar(40) NOT NULL COMMENT '控制密钥',\
		`app_time` datetime NOT NULL COMMENT '申请时间',\
		`app_status` bit(1) NOT NULL COMMENT '申请状态',\
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
		bodyroot["status"] = 1;
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
		snprintf((char*)sql.c_str(), sql.size(), "insert into ctuser(uphone, uname, upassword, ubalance, Uregister_time) VALUES('%s', '%s', '%s', %lf, '%s');\0\0", root[fields[0]].asString().c_str(), root[fields[1]].asString().c_str(), root[fields[2]].asString().c_str(), root[fields[3]].asDouble(), root[fields[8]].asString().c_str());
		LOG_INFO << "registerDispose insert user sql:[" << sql.c_str() << "]";
		m_logFile->flush();
		if (false == m_db.Execute(sql)) { // 向数据库中插入数据
			LOG_ERROR << "sql 执行失败";
			m_logFile->flush();
			break;
		}
		// 查询用户id
		snprintf((char*)sql.c_str(), sql.size(), "select Uid from `ctuser` where Uphone='%s';\0\0", root[fields[0]].asString().c_str());
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
			Records rec = res->GetRecords();
			uid = rec[0][0].asLONGLONG();
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
	Buffer phone = url["phone"];         // 手机号
	Buffer password = url["password"];   // 密码
	LOG_INFO << "login: phone [" << phone.c_str() << "] password [" << password.c_str() << "]";
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
			Records rec = res->GetRecords();
			upassword = rec[0][0];
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
		std::string fields[] = { "equipName", "width", "height","phone","password" };
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


		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "select Upassword from ctuser where Uphone='%s';\0\0", phone.c_str());
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
			Records rec = res->GetRecords();
			upassword = rec[0][0];
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
		it->second.eInfo.eStatus = 1;  // 当前为空闲状态
		LOG_INFO << "设备的尺寸比例: width:" << it->second.eInfo.width << "height:" << it->second.eInfo.height;
		m_logFile->flush();
		body["message"] = "success";
		body["status"] = 0;
	} while (false);
BREAK:
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
			eItem["name"] = it->second.eInfo.equipName;
			eItem["ip"] = inet_ntoa(it->second.clientAddr.sin_addr);
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
		std::string fields[] = { "ctrlerPhone", "bectrlEquipName", "startCtrlTime" };
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
		std::string eName = root[fields[1]].asString();
		std::string phone = root[fields[0]].asString();
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
						if (bind(sock, (sockaddr*)&recvImageAddr, sizeof(recvImageAddr)) < 0) {
							LOG_ERROR << "服务器端口绑定失败 " << SEInfo.recvImagePort;
							m_logFile->flush();
							close(sock);
							break;
						}
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
						bodyRoot["CtrlTime"] = 5;
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

int CCrossTimeServer::endEquipmentCtrlDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket)
{
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	Json::Value bodyRoot;   // 返回数据的json结构
	bodyRoot["message"] = "当前设备没被控制";
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
					}
					else {
						// 停止设备控制状态
						auto ret = m_mbEqu.find(eName);
						if (ret == m_mbEqu.end()) {
							LOG_WARN << "当前设备开启的线程没被保存";
							m_logFile->flush();
							break;
						}
						// 加判断，看当前发起关闭请求的用户是不是开启控制的用户
						auto phoneIt = parser.Headers().find("Authorization");
						if (phoneIt == parser.Headers().end()) {
							LOG_ERROR << "指定字段不存在（token）";
							m_logFile->flush();
							break;
						}
						std::string phone = phoneIt->second;
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
						else m_db.FreeResult(res);
						if (id == "") {
							// 查询不到用户的id
							break;
						}
						if (ret->second.ctrlerId != id) {
							// 停止控制设备和开启控制设备的用户不一样
							break;
						}
						// 通知被控端停止屏幕数据发送
						char str[] = "停止控制";
						CPacket pack(2, 2, str, sizeof(str));
						ssize_t len = send(it->first, pack.outStr(), pack.Size(), 0);
						if (len <= 0) {
							LOG_ERROR << "数据发送失败";
							m_logFile->flush();
							break;
						}
						if (it->second.eInfo.thread < 0) {
							LOG_WARN << "当前设备开启的线程没被保存";
							m_logFile->flush();
							break;
						}
						// 停止设备控制
						SOCKET sock = ret->second.recvImageSock;
						SOCKET clientsock = ret->second.sendImageClientSock;
						ret->second.recvImageSock = INVALID_SOCKET;
						ret->second.sendImageClientSock = INVALID_SOCKET;
						if (clientsock != INVALID_SOCKET) {
							close(clientsock);
						}
						else {
							LOG_ERROR << "不正常表现， 发送的图片套接字没有";
							// 这块就应该直接将线程给销毁掉
							close(clientsock);
							close(sock);
							it->second.eInfo.eStatus = 1;
							it->second.eInfo.thread = -1;
							break;
						}
						if (sock != INVALID_SOCKET)
							close(sock);
						else {
							LOG_ERROR << "不正常表现， 接收图片套接字为空";
							close(clientsock);
							close(sock);
							it->second.eInfo.eStatus = 1;
							it->second.eInfo.thread = -1;
							break;
						}

						m_mbEqu.erase(ret);

						LOG_INFO << "开始等待屏幕监控线程结束";
						m_logFile->flush();

						void* retval = NULL;
						struct timespec abs = {};
						abs.tv_sec = 5;
						if (ETIMEDOUT == pthread_timedjoin_np(it->second.eInfo.thread, &retval, &abs)) {
							// 等待线程结束超时
							LOG_ERROR << "以强制关闭监控线程";
							pthread_cancel(it->second.eInfo.thread); // 强制结束线程
						}

						// pthread_join(it->second.eInfo.thread, NULL); // 等待线程结束【无限等待】

						LOG_INFO << "接收图片线程结束";
						m_logFile->flush();

						it->second.eInfo.eStatus = 1;
						it->second.eInfo.thread = -1;

						bodyRoot["message"] = "已停止设备控制";
						bodyRoot["status"] = 1;
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
	// 请求数据
	Buffer buffer = parser.Body();
	// 解析请求体数据
	Json::Value root;
	Json::Reader reader;
	if (false == reader.parse(buffer, root)) {
		// 解析失败
		return -1;
	}

	Json::Value bodyRoot;
	std::string fields[] = { "bectrlEquipName", "EventType", "MousePoint","operation","Value" };
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
					CPacket pack(3, 3, str, sizeof(str));
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
		snprintf((char*)sql.c_str(), sql.size(), "select Uid from ctuser where Uphone='%s';\0\0", phone.c_str());
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
			Records rec = res->GetRecords();
			uid = rec[0][0].asLONGLONG();
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
		snprintf((char*)sql.c_str(), sql.size(), "select question1, question2 from revert_password where Uid=%ld;\0\0", uid);
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
			Records rec = res->GetRecords();
			answer1 = rec[0][0];
			answer2 = rec[0][1];
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
		snprintf((char*)sql.c_str(), sql.size(), "select Uid, Upassword from ctuser where Uphone='%s';\0\0", phone.c_str());
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
			Records rec = res->GetRecords();
			uid = rec[0][0].asLONGLONG();
			password = rec[0][1];
			m_db.FreeResult(res);
		}
		else {
			LOG_ERROR << "数据获取失败";
			m_logFile->flush();
			break;
		}
		// 然后通过用户id去找设定的两个问题的答案
		snprintf((char*)sql.c_str(), sql.size(), "select answer1, answer2 from revert_password where Uid=%ld;\0\0", uid);
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
			Records rec = res->GetRecords();
			std_answer1 = rec[0][0];
			std_answer2 = rec[0][1];
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
			// 没有token数据
			LOG_ERROR << "信息获取失败";
			m_logFile->flush();
			break;
		}
		std::string phone = itt->second.c_str();
		// 从数据库中获取用户的信息
		std::string sql(512, '\0');
		snprintf((char*)sql.c_str(), sql.size(), "select Uname, Uavatar, Upassword, Usex, Uaddr, Uregister_time, Ubalance from ctuser where Uphone = '%s';\0\0", phone.c_str());
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
		std::string name, avatar, password, sex, addr, registerTime;
		double balance; // 余额
		if (res->GetRecordsSize() > 0) {
			Records rec = res->GetRecords();
			name = rec[0][0];
			avatar = rec[0][1];
			password = rec[0][2];
			sex = rec[0][3];
			addr = rec[0][4];
			registerTime = rec[0][5];
			balance = rec[0][6].asDOUBLE();
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
		bodyRoot["avatar"] = avatar;
		bodyRoot["name"] = name;
		bodyRoot["password"] = password;
		bodyRoot["sex"] = sex;
		bodyRoot["addr"] = addr;
		bodyRoot["phone"] = phone;
		bodyRoot["registerTime"] = registerTime;
		bodyRoot["balance"] = balance;
	} while (false);
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
			LOG_ERROR << "指定字段不存在（token）";
			m_logFile->flush();
			break;
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

		std::string fields[] = { "avatar","name","password","sex","addr","phone" };
		std::string db_fields[] = { "Uavatar","Uname","Upassword", "Usex", "Uaddr", "Uphone" };
		std::string fieldsValue[sizeof(fields) / sizeof(*fields)] = { "" };
		bool bupdate = false;
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
			Records rec = res->GetRecords();
			uid = rec[0][0].asLONGLONG();
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
			Records rec = res->GetRecords();
			content = rec[0][0];
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
			Records rec = res->GetRecords();
			for (size_t i = 0; i < recSize; i++) {
				Item["id"] = (int)rec[i][0].asLONGLONG();
				Item["title"] = rec[i][1];
				Item["message"] = rec[i][2];
				Item["createdAt"] = rec[i][3];
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

void* CCrossTimeServer::ScreenMonitorThreadEnt(void* arg)
{
	screenArgs* pArgs = (screenArgs*)arg;
	screenArgs args = *pArgs;
	delete pArgs;
	CCrossTimeServer* thiz = args.thiz;
	thiz->ScreenMonitorThreadMain(args.beSocket, args.eName);
	return nullptr;
}

int CCrossTimeServer::ScreenMonitorThreadMain(SOCKET beSock, const std::string& eName)
{
	int count = 6; // 获取6次
	while (count--) {
		auto it = m_mbEqu.find(eName);
		if (it == m_mbEqu.end()) {
			// 没找到当前这个设备的信息
			return -1;
		}
		if (it->second.recvImageSock == INVALID_SOCKET) {
			// 还没有更新过来
			continue;
		}
		else {
			break;
		}
		sleep(5);
	}
	auto it = m_mbEqu.find(eName);
	if (it == m_mbEqu.end()) {
		return -1;
	}
	SOCKET sock = it->second.recvImageSock;
	if ((count <= 0) && (sock == INVALID_SOCKET)) {
		// 可能存在问题
		return -1;
	}
	std::string path = it->second.imageDir.c_str();
	unsigned short port = it->second.recvImagePort;

	// 通知被控端连接服务器
	char str[] = "开始控制";
	CPacket pack(1, port, str, sizeof(str));
	ssize_t len = send(beSock, pack.outStr(), pack.Size(), 0);
	if (len <= 0) {
		LOG_ERROR << "数据发送失败";
		m_logFile->flush();
		close(sock);
	}

	sockaddr_in clientAddr = {};
	int clientSock = INVALID_SOCKET;
	do {
		socklen_t AddrLen = sizeof(clientAddr);
		clientSock = accept(sock, (sockaddr*)&clientAddr, &AddrLen);
		if (clientSock < 0) {
			// 套接字连接失败
			LOG_ERROR << "套接字连接失败";
			m_logFile->flush();
			break;
		}
		// 被控端连接上来
		m_mbEqu.find(eName)->second.sendImageClientSock = clientSock;
		// 将客户端套接字设置为非阻塞模式
		int flags = 0;
		if ((flags = fcntl(clientSock, F_GETFL, 0)) < 0) {
			LOG_ERROR << "获取套接字配置选项失败";
			m_logFile->flush();
			break;
		}
		flags |= O_NONBLOCK;
		if (fcntl(clientSock, F_SETFL, flags) < 0) {
			LOG_ERROR << "设置套接字配置选项失败";
			m_logFile->flush();
			break;
		}

		std::string Buffer;
		Buffer.resize(BUFFER_SIZE);
		char* pData = (char*)Buffer.c_str();
		long long index = 0;
		int count = 0;
		while (true) {
			long long len = recv(clientSock, pData + index, BUFFER_SIZE - index, 0);
			if (len <= 0) {
				if (EAGAIN == errno) { // 没有错误，只是非阻塞
					if (index > 0) {
						// 证明里面还有数据
						len = 0;
					}
					else if (len == 0) {
						break;
					}
					else {
						usleep(50000);
						continue;
					}
				}
				else {
					LOG_ERROR << "监控线程退出";
					m_logFile->flush();
					break;
				}
			}
			index += len;
			// TODO:处理命令
			len = index;
			if (len == 0) {
				count++;
				if (count >= 10)break;
				continue;
			}
			count = 0;
			CPacket m_packet(pData, len);
			if (len > 0) {
				memmove(pData, pData + len, BUFFER_SIZE - len);
				switch (m_packet.GetCmd()) {
				case 1: // 屏幕数据
					int fd = open(path.c_str(), O_RDWR | O_CREAT, 0666);
					if (fd < 0) {
						LOG_ERROR << "文件创建失败！[" << path.c_str() << "]";
						m_logFile->flush();
						goto BREAK;
					}
					ssize_t ret = write(fd, m_packet.GetData(), m_packet.GetDataSize());
					LOG_INFO << "图片写入字节数 ret=" << ret << " 图片位置:[" << path << "]";
					m_logFile->flush();
					if (ret < 0) {
						LOG_ERROR << "写入错误 ret = " << ret;
						m_logFile->flush();
					}
					close(fd);
					break;
				}
			}
			else if (len < 0) {
				// 数据包校验和失败
				len = -1 * len;
				memmove(pData, pData + len, BUFFER_SIZE - len);
			}
			index -= len;
		}
	} while (false);
BREAK:
	// 设置端口复用
	int enable = 1;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if (ret < 0) {
		LOG_INFO << "setsockopt ret=" << ret << "errno=" << errno;
		m_logFile->flush();
	}
	close(clientSock);
	close(sock);
	LOG_INFO << "图片接收线程结束";
	m_logFile->flush();
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

void CCrossTimeServer::outputFunc(const char* msg, int len)
{
	m_logFile->append(msg, len);
}

void CCrossTimeServer::flushFunc()
{
	m_logFile->flush();
}
