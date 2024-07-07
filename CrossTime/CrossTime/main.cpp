#include <cstdio>
#include "CrossTimeServer.h"
#include "FilesServer.h"


int curlTest()
{

	/*if (1) {

		return;
	}*/

	// https://api.map.baidu.com/location/ip?ip=111.206.214.37&coor=bd09ll&ak=kEmx0DECl6ZywbdDBRbhxJLiIOX7It4P

	// 设置请求参数字段
	//std::map<std::string, std::string> params;
	//params["ip"] = "1.190.146.248";
	//params["coor"] = "bd09ll";
	//params["ak"] = "kEmx0DECl6ZywbdDBRbhxJLiIOX7It4P";
	//std::string ack = CTools::CurlTool("http://api.map.baidu.com", "/location/ip", params);

	//// 解析请求体数据
	//Json::Value root;
	//Json::Reader reader;
	//if (false == reader.parse(ack, root)) {
	//	// 解析失败
	//	return -1;
	//}
	//int status = root["status"].asInt();
	//if (status != 0)return -2;
	//std::string address = root["address"].asString();
	//std::string address_con = root["content"]["address"].asString();
	//std::string con_add_det = root["content"]["address_detail"]["city"].asString();   // 这个是市
	//std::string con_add_pro = root["content"]["address_detail"]["province"].asString(); // 这个是省
	//std::string longitude = root["content"]["point"]["x"].asString();  // 经度
	//std::string latitude = root["content"]["point"]["y"].asString();   // 纬度  
	//std::cout << "address:" << address << "\naddress_con:" << address_con
	//	<< "\ncon_add_det:" << con_add_det
	//	<< "\ncon_add_pro:" << con_add_pro
	//	<< "\n经度:" << longitude
	//	<< "\n纬度:" << latitude
	//	<< std::endl;
	//std::cout << root.toStyledString().c_str() << std::endl;


	//// https://api.map.baidu.com/reverse_geocoding/v3/?ak=kEmx0DECl6ZywbdDBRbhxJLiIOX7It4P&output=json&coordtype=wgs84ll&location=31.225696563611,121.49884033194
	//params.clear();
	//std::string location(512, '\0');
	//snprintf((char*)location.c_str(), location.size(), "37.889025,112.566995");
	//// params["ip"] = "1.190.146.248";
	//params["location"] = location.c_str();
	//params["coordtype"] = "wgs84ll";
	//params["output"] = "json";
	//params["ak"] = "kEmx0DECl6ZywbdDBRbhxJLiIOX7It4P";
	//ack = CTools::CurlTool("http://api.map.baidu.com", "/reverse_geocoding/v3/", params);

	//root.clear();
	//if (false == reader.parse(ack, root)) {
	//	// 解析失败
	//	return -1;
	//}
	//status = root["status"].asInt();
	//if (status != 0)return -2;
	//con_add_pro = root["result"]["addressComponent"]["province"].asString();
	//con_add_det = root["result"]["addressComponent"]["city"].asString();
	//std::cout << "省：" << con_add_pro << " 市：" << con_add_det << "\n";
	//std::cout << root.toStyledString().c_str() << std::endl;




	/*
	// 百度API
	params["location"] = location.c_str();
	params["coordtype"] = "wgs84ll";
	params["output"] = "json";
	params["ak"] = "kEmx0DECl6ZywbdDBRbhxJLiIOX7It4P";
	std::string ack = CTools::CurlTool("http://api.map.baidu.com", "/reverse_geocoding/v3/", params);*/

	//std::map<std::string, std::string> params;
	//std::string location(512, '\0');
	//snprintf((char*)location.c_str(), location.size(), "37.889025,112.566995");
	//params["location"] = location.c_str();
	//params["get_poi"] = "0";
	//params["key"] = "OA4BZ-FX43U-E5VV2-45M6S-C4HD3-NIFFI";
	//params["output"] = "json";
	//std::string ack = CTools::CurlTool("http://apis.map.qq.com", "/ws/geocoder/v1/", params);
	//Json::Value root;
	//Json::Reader reader;
	//if (false == reader.parse(ack, root)) {
	//	// 解析失败
	//	return -1;
	//}
	//int status = root["status"].asInt();
	//if (status != 0) {
	//	std::cout << ack << std::endl;
	//	return -2;
	//}
	//std::string con_add_pro = root["result"]["address_component"]["province"].asString();
	//std::string con_add_det = root["result"]["address_component"]["city"].asString();
	//std::cout << "省：" << con_add_pro << " 市：" << con_add_det << "\n";
	//std::cout << root.toStyledString().c_str() << std::endl;

	std::string ip = "1.190.146.248";
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
		return 0;
	}
	int status = root["status"].asInt();
	if (status != 0) {
		return 0;
	}

	// std::string con_add_det = root["content"]["address_detail"]["city"].asString();   // 这个是市
	// http://api.lolimi.cn/API/weather/?city=北京市
	params.clear();
	params["city"] = root["content"]["address_detail"]["city"].asString();
	ack = CTools::CurlTool("http://api.lolimi.cn", "/API/weather/", params, url);
	std::cout << ack.c_str() << std::endl;

	return 0;
}

#include "WebSocket.h"

void on_connect(websocketpp::connection_hdl hdl);
void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg);

WebSocketServer wsServer(9002, on_connect, on_message);

std::map<std::string, std::weak_ptr<void>> m_mClient;   // 连接上来的客户端映射表

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg)
{/*
	std::cout << "on_message called with hdl: " << hdl.lock().get()
		<< " and message: " << msg->get_payload()
		<< std::endl;*/

	std::string data = msg->get_payload();
	data += '\0';

	printf("<%s>(%d):[info]收到了客户端的数据 data:[%s] hdl:%p\r\n", __FILE__, __LINE__, data.c_str(), hdl);
	if ((data[0] != '{') && (data[data.size() - 1] != '}')) {
		// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
		// 这个可能是传输文件的数据包
		char ToCtrl[20] = "";
		if (data.size() > sizeof(ToCtrl)) {
			memcpy(ToCtrl, data.c_str(), sizeof(ToCtrl));
		}
		auto it = m_mClient.find(ToCtrl);
		if (it == m_mClient.end()) {
			printf("<%s>(%d):[error]没有你要发送的客户端 eName:[%s]\r\n", __FILE__, __LINE__, ToCtrl);
			// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
			return;
		}
		std::string Data(data.size() - sizeof(ToCtrl) - 1, '\0');
		memcpy((char*)Data.c_str(), data.c_str() + sizeof(ToCtrl), Data.size());
		// printf("<%s>(%d):[INFO]:数据大小为 %d  data:[%s]\r\n", __FILE__, __LINE__, data.size(), Data.c_str());

		// printf("<%s>(%d):[INFO]data:[%s]\r\n", __FILE__, __LINE__, Data.c_str());

		try {
			wsServer.send(it->second, Data, websocketpp::frame::opcode::binary);
			// sleep(1);
		}
		catch (websocketpp::exception const& e) {
			std::cout << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
		}
		catch (...) {
			std::cout << "error " << std::endl;
		}
		return;
	}

	// data是json数据
	// 进行json数据解析
	Json::Value root;
	Json::Reader reader;
	if (false == reader.parse(data, root)) {
		// 解析失败
		printf("<%s>(%d):[error]json 数据解析失败 data:[%s]\r\n", __FILE__, __LINE__, data.c_str());
		// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
		return;
	}

	std::string fields[] = { "auth","equip","EName","toCtrl","opType" };
	if (false == root.isMember(fields[4])) {
		printf("<%s>(%d):[error]类型不匹配，没有opType类型\r\n", __FILE__, __LINE__);
		// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
		return;
	}
	std::string opType = root[fields[4]].asString();
	// auth
	if (true == root.isMember(fields[0])) {
		// 表示这个是控制端来的消息，要转发给指定控制端
		if (opType == "0") {
			// 这是一个控制端的上线包
			std::string  auth = root[fields[0]].asString();
			auto it = m_mClient.find(auth);
			if (it != m_mClient.end()) {
				m_mClient.erase(it);
			}
			m_mClient.insert(std::pair<std::string, websocketpp::connection_hdl >(auth, hdl));
			printf("<%s>(%d):[info]:控制端已上线 hdl:%p\r\n", __FILE__, __LINE__, hdl);
			// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
			return;
		}

		// 找设备名
		if (false == root.isMember(fields[2])) {
			// 控制端必须含有这个字段
			printf("<%s>(%d):[error]控制端数据不含有EName字段\r\n", __FILE__, __LINE__);
			// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
			return;
		}
		std::string  eName = root[fields[2]].asString();
		printf("<%s>(%d):[info]EName:[%s]\r\n", __FILE__, __LINE__, eName.c_str());
		auto it = m_mClient.find(eName);
		if (it == m_mClient.end()) {
			printf("<%s>(%d):[error]没有你要发送的客户端 eName:[%s]\r\n", __FILE__, __LINE__, eName.c_str());
			// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
			return;
		}
		printf("<%s>(%d):[info]:将要控制端消息发送给被控端 hdl:%p\r\n", __FILE__, __LINE__, it->second);

		try {
			wsServer.send(it->second, msg->get_payload(), msg->get_opcode());
		}
		catch (websocketpp::exception const& e) {
			std::cout << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
		}
		catch (...) {
			std::cout << "error " << std::endl;
		}

		// wsServer.send(it->second, root.toStyledString(), websocketpp::frame::opcode::binary);
		printf("<%s>(%d):[info]:已将控制端消息发送给被控端 hdl:%p\r\n", __FILE__, __LINE__, it->second);
	}
	// equip
	else if (true == root.isMember(fields[1])) {
		if (opType == "0") {
			// 这是一个被控端的上线包

			std::string  equip = root[fields[1]].asString();
			auto it = m_mClient.find(equip);
			if (it != m_mClient.end()) {
				m_mClient.erase(it);
			}
			m_mClient.insert(std::pair<std::string, websocketpp::connection_hdl >(equip, hdl));
			printf("<%s>(%d):[info]:被控端已上线 hdl:%p\r\n", __FILE__, __LINE__, hdl);
			// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
			return;
		}
		// 表示这个是被控端来的消息，要转发给指定控制端
		if (false == root.isMember(fields[3])) {
			// 被控端必须含有这个字段
			printf("<%s>(%d):[error]被控端数据不含有toCtrl字段\r\n", __FILE__, __LINE__);
			// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
			return;
		}
		std::string auth = root[fields[3]].asString();
		auto it = m_mClient.find(auth);
		if (it == m_mClient.end()) {
			printf("<%s>(%d):[error]没有你要发送的客户端 auth:[%s]\r\n", __FILE__, __LINE__, auth.c_str());
			// wsServer.send(hdl, "error", websocketpp::frame::opcode::text);
			return;
		}
		root.removeMember(fields[3]);  // 把这个字段删掉不需要让控制端知道
		std::string  retData = root.toStyledString();
		wsServer.send(it->second, retData, msg->get_opcode());
		printf("<%s>(%d):[info]:已将被控端消息发送给控制端 hdl:%p\r\n", __FILE__, __LINE__, it->second);
	}
	else {
		return;
	}


	//if (data == "ctrl") {
	//	// 这个是控制端
	//	printf("这个是控制端 %p\r\n", ctrl);
	//	ctrl = hdl;
	//	try {
	//		s->send(hdl, "ok", websocketpp::frame::opcode::text);
	//	}
	//	catch (websocketpp::exception const& e) {
	//		std::cout << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
	//	}
	//	bCtrl = true;
	//}
	//else if (data == "bectrl") {
	//	// 这个是被控端
	//	printf("这个是被控端 %p\r\n", bectrl);
	//	bectrl = hdl;
	//	try {
	//		s->send(hdl, "ok", websocketpp::frame::opcode::text);
	//	}
	//	catch (websocketpp::exception const& e) {
	//		std::cout << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
	//	}
	//	bBectrl = true;
	//}

	//if (hdl.lock().get() == ctrl.lock().get()) {
	//	// 吧这个东西发给被控端
	//	if (bBectrl == true) {
	//		printf("控制端发来的消息，并发给了被控端\r\n");
	//		try {
	//			s->send(bectrl, msg->get_payload(), msg->get_opcode());
	//		}
	//		catch (websocketpp::exception const& e) {
	//			std::cout << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
	//		}
	//	}
	//}
	//else if (hdl.lock().get() == bectrl.lock().get()) {
	//	// 吧这个东西发个控制端
	//	if (bCtrl == true) {
	//		printf("被控制端发来的消息，并发给了控制端\r\n");
	//		try {
	//			s->send(ctrl, msg->get_payload(), msg->get_opcode());
	//		}
	//		catch (websocketpp::exception const& e) {
	//			std::cout << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
	//		}

	//	}
	//}

	//printf("数据：%s\r\n", msg->get_payload().c_str());

	//// check for a special command to instruct the server to stop listening so
	//// it can be cleanly exited.
	//if (msg->get_payload() == "stop-listening") {
	//	s->stop_listening();
	//	return;
	//}

	//try {
	//	// s->send(hdl, msg->get_payload(), msg->get_opcode());
	//}
	//catch (websocketpp::exception const& e) {
	//	std::cout << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
	//}
}

void on_connect(websocketpp::connection_hdl hdl)
{
	// 向服务器发送消息
		// con->send("Hello, Server!");
	// wsServer.send(hdl, "Hello, My is Server!", websocketpp::frame::opcode::text);
	// ws_server->send(hdl, "Hello, My is Server!", websocketpp::frame::opcode::text);
}

int wsTest()
{
	LOG_ERROR << "websocketServer启动成功";
	CCrossTimeServer::m_logFile->flush();
	wsServer.Start();
	LOG_ERROR << "websocketServer停止运行";
	CCrossTimeServer::m_logFile->flush();
	return 0;
}

int main(int argc, char* argv[])
{
	CTools::restart(argc, argv);

	/*if ((argc > 1) && (strcmp(argv[1], "websocketServer") == 0)) {
		if (CTools::SwitchDeamon() >= 0) {
			printf("<%s>(%d):开启websocket服务器\n", __FILE__, __LINE__);
			wsTest();
			return 0;
		}
		else {
			printf("<%s>(%d):守护进程创建失败\r\n", __FILE__, __LINE__);
			LOG_ERROR << "守护进程创建失败";
			CCrossTimeServer::m_logFile->flush();
		}
	}*/

	/*curlTest();
	return 0;*/


	if (CTools::SwitchDeamon() >= 0) {

		CCrossTimeServer server;  // web服务器
		// websocket服务器
		CProcess procWebSocketServer;
		if (procWebSocketServer.SetEntryFunction(wsTest) != 0) {
			LOG_ERROR << "进程入口函数创建失败";
			CCrossTimeServer::m_logFile->flush();
			return -1;
		}
		if (procWebSocketServer.CreateSubProcess() != 0) {
			LOG_ERROR << "文件服务器进程开启失败";
			CCrossTimeServer::m_logFile->flush();
			return -1;
		}

		/*
		 CProcess procFileServer;  // 文件服务器
		 if (procFileServer.SetEntryFunction(CFilesServer::CreateFileServer, &procFileServer) != 0) {
			LOG_ERROR << "进程入口函数创建失败";
			CCrossTimeServer::m_logFile->flush();
			return -1;
		}
		if (procFileServer.CreateSubProcess() != 0) {
			LOG_ERROR << "文件服务器进程开启失败";
			CCrossTimeServer::m_logFile->flush();
			return -1;
		}*/

		server.Start();
		// procFileServer.SendFD(0); // 通知服务器线程退出
	}
	else {
		printf("<%s>(%d):守护进程创建失败\r\n", __FILE__, __LINE__);
		LOG_ERROR << "守护进程创建失败";
		CCrossTimeServer::m_logFile->flush();
	}
	return 0;
}