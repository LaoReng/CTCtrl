#pragma once
#include <iostream>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/server.hpp>
#include "Tools.h"

typedef websocketpp::client<websocketpp::config::asio_client> client;  // webSocketpp客户端类型
typedef websocketpp::server<websocketpp::config::asio> server;         // webSocketpp服务端类型
typedef void(*conFunc)(websocketpp::connection_hdl); // 连接cb函数
typedef void(*cRecvMessageFunc)(client* c, websocketpp::connection_hdl, client::message_ptr);  // 客户端接收到数据的回调函数
typedef void(*sRecvMessageFunc)(server* c, websocketpp::connection_hdl, client::message_ptr);  // 服务器接收到数据的回调函数

// 只需要设置发送消息，因为自动接收消息
class WebSocketClient
{
public:
	// 构造函数
	WebSocketClient(const std::string& serverUri, conFunc pConFunc = NULL, cRecvMessageFunc pRMFunc = NULL)
		: uri(serverUri)
		, m_pRMFunc(pRMFunc)
	{
		m_pConFunc = pConFunc;
		con = NULL;
		if (m_pRMFunc)
			ws_client.set_message_handler(std::bind(m_pRMFunc, &ws_client, std::placeholders::_1, std::placeholders::_2));
		if (m_pConFunc)
			ws_client.set_open_handler(m_pConFunc);
		ws_client.set_fail_handler(std::bind(&on_fail, &ws_client, std::placeholders::_1));
	}
	// 析构函数
	~WebSocketClient() {
		// ws_client.close();
		if (connection_successful)
			ws_client.close(con->get_handle(), websocketpp::close::status::going_away, "");
	}
	// 初始化
	bool Init() {
		ws_client.init_asio();
		websocketpp::lib::error_code ec;
		con = ws_client.get_connection(uri, ec);
		if (ec) {
			std::cout << "Could not create connection because: " << ec.message() << std::endl;
			return false;
		}
		return true;
	}
	// 连接服务器
	bool connect() {
		ws_client.connect(con);
		printf("连接失败的返回值：%p\r\n", con.get());
		// con->get_handle()
		return true;
	}
	// 开启客户端
	int start() {
		return (int)ws_client.run();
	}
	// 停止客户端
	int stop() {
		ws_client.stop();
		return 0;
	}

	// TODO:让用户去绑定回调事件

	// 发送消息
	int send(const std::string& data, websocketpp::frame::opcode::value op) {
		ws_client.send(con->get_handle(), data, op);
		return 0;
	}
	// 判断客户端是否连接成功
	bool bConnect() {
		return connection_successful;
	}
private:
	/// <summary>
	/// 连接服务器成功后回调函数
	/// </summary>
	/// <param name="hdl"></param>
	static void on_connect(websocketpp::connection_hdl hdl)
	{
		connection_successful = true;
		printf("成功连接服务器 hdl:%p\r\n", hdl.lock().get());
		if (m_pConFunc)
			m_pConFunc(hdl);
	}
	/// <summary>
	/// 出错后回调函数
	/// </summary>
	/// <param name="c"></param>
	/// <param name="hdl"></param>
	static void on_fail(client* c, websocketpp::connection_hdl hdl) {
		connection_successful = false;
		std::cout << "连接服务器失败" << std::endl;
	}
private:
	client::connection_ptr con;
	std::string            uri;   // 要连接的服务器ws地址
	client                 ws_client;  // webSocket客户端
	static bool            connection_successful;  // 是否连接成功服务器
	static conFunc         m_pConFunc;     // 连接成功后回调函数，由用户指定，主要不可以加循环
	cRecvMessageFunc m_pRMFunc; // 接收到数据的回调函数
};



class WebSocketServer
{
public:
	// 构造函数
	WebSocketServer(
		short bindPort,
		conFunc pConFunc = NULL,
		sRecvMessageFunc pRMFunc = NULL,
		bool bUseDefaultLogger = false)
		: m_bingPort(bindPort)
	{
		ws_server = new server;
		m_pConFunc = pConFunc;
		m_pRMFunc = pRMFunc;
		if (ws_server == NULL) {
			// 服务器创建失败
			printf("<%s>(%d):服务器创建失败\r\n", __FILE__, __LINE__);
			exit(0);
		}
		if (bUseDefaultLogger) {
			// Set logging settings
			ws_server->set_access_channels(websocketpp::log::alevel::all);
			ws_server->clear_access_channels(websocketpp::log::alevel::frame_payload);
		}
		// Register our message handler
		ws_server->set_message_handler(std::bind(m_pRMFunc, ws_server, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		ws_server->set_open_handler(on_connect);
		// Initialize Asio
		ws_server->init_asio();
	}
	// 析构函数
	~WebSocketServer() {
		if (ws_server) {
			// echo_server->close()
			delete ws_server;
			ws_server = NULL;
		}
	}
	// 开启服务器
	int Start() {
		try {
			// 监听指定的端口
			ws_server->listen(m_bingPort);
			// 开启服务器连接池
			ws_server->start_accept();
			// 启动ASIO io服务运行循环
			ws_server->run();
		}
		catch (websocketpp::exception const& e) {
			std::cout << e.what() << std::endl;
			return -1;
		}
		catch (...) {
			std::cout << "other exception" << std::endl;
			return -2;
		}
		return 0;
	}
	// 停止服务器
	int Stop() {
		ws_server->stop();
		return 0;
	}
	// 发送消息
	int send(websocketpp::connection_hdl hdl, const std::string& data, websocketpp::frame::opcode::value op) {
		ws_server->send(hdl, data, op);
		return  0;
	}
private:
	/// <summary>
	/// 客户端连接成功时回调函数
	/// </summary>
	/// <param name="hdl"></param>
	static void on_connect(websocketpp::connection_hdl hdl) {
		// server::connection_ptr con = echo_server.get_con_from_hdl(hdl);

		printf("客户端成功连接成功 hdl:%p\r\n", hdl.lock().get());


		if (m_pConFunc)
			m_pConFunc(hdl); // 用户自定义处理部分
	}
private:
	static server* ws_server;   // webSocket服务端
	unsigned short                             m_bingPort;  // webSocket绑定的端口
	static conFunc                             m_pConFunc;  // 连接成功后回调函数
	sRecvMessageFunc                            m_pRMFunc;   // 接收到数据后回调函数
	
};