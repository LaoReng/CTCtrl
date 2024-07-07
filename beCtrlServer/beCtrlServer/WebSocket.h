#pragma once
#include <iostream>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/server.hpp>
#include "Tools.h"

typedef websocketpp::client<websocketpp::config::asio_client> client;  // webSocketpp�ͻ�������
typedef websocketpp::server<websocketpp::config::asio> server;         // webSocketpp���������
typedef void(*conFunc)(websocketpp::connection_hdl); // ����cb����
typedef void(*cRecvMessageFunc)(client* c, websocketpp::connection_hdl, client::message_ptr);  // �ͻ��˽��յ����ݵĻص�����
typedef void(*sRecvMessageFunc)(server* c, websocketpp::connection_hdl, client::message_ptr);  // ���������յ����ݵĻص�����

// ֻ��Ҫ���÷�����Ϣ����Ϊ�Զ�������Ϣ
class WebSocketClient
{
public:
	// ���캯��
	WebSocketClient(const std::string& serverUri, conFunc pConFunc = NULL, cRecvMessageFunc pRMFunc = NULL)
		: uri(serverUri)
		, m_pRMFunc(pRMFunc)
	{
		m_pConFunc = pConFunc;
		//con = NULL;
		if (m_pRMFunc)
			ws_client.set_message_handler(std::bind(m_pRMFunc, &ws_client, std::placeholders::_1, std::placeholders::_2));
		if (m_pConFunc)
			ws_client.set_open_handler(on_connect);
		ws_client.set_fail_handler(std::bind(&on_fail, &ws_client, std::placeholders::_1));
		ws_client.init_asio();
	}
	// ��������
	~WebSocketClient() {
		// ws_client.close();
		// ws_client.stop();
		if (connection_successful) {
			Stop();
			/*client::connection_ptr con;
			websocketpp::lib::error_code ec;
			con = ws_client.get_connection(uri, ec);
			if (ec) {
				std::cout << "Could not create connection because: " << ec.message() << std::endl;
				return;
			}*/
			ws_client.close(m_hdl, websocketpp::close::status::going_away, "");  // con->get_handle()
		}

	}
	// ���ӷ�����
	bool connect() {
		client::connection_ptr con;
		websocketpp::lib::error_code ec;
		con = ws_client.get_connection(uri, ec);
		if (ec) {
			std::cout << "Could not create connection because: " << ec.message() << std::endl;
			return false;
		}
		ws_client.connect(con);
		printf("����ʧ�ܵķ���ֵ��%p\r\n", con.get());
		// con->get_handle()
		return true;
	}
	// �����ͻ���
	int Start() {
		return (int)ws_client.run();
	}
	// ֹͣ�ͻ���
	int Stop() {
		ws_client.stop();
		return 0;
	}

	// TODO:���û�ȥ�󶨻ص��¼�

	// ������Ϣ
	int send(const std::string& data, websocketpp::frame::opcode::value op) {
		try {
			if (connection_successful) {
				ws_client.send(m_hdl, data, op);  // con->get_handle()
			}
			else {
#ifndef TRACE
				printf("<%s>(%d):δ������websocket������\r\n", __FILE__, __LINE__);
#else
				TRACE(_T("δ������websocket������\r\n"));
#endif // !TRACE
				return -1;
			}
		}
		catch (websocketpp::exception const& e) {
			std::cout << e.what() << std::endl;
			return -1;
		}
		return 0;
	}
	// �жϿͻ����Ƿ����ӳɹ�
	bool bConnect() {
		return connection_successful;
	}
private:
	/// <summary>
	/// ���ӷ������ɹ���ص�����
	/// </summary>
	/// <param name="hdl"></param>
	static void on_connect(websocketpp::connection_hdl hdl)
	{
		connection_successful = true;
		m_hdl = hdl;
#ifndef TRACE
		printf("�ɹ����ӷ����� hdl:%p\r\n", hdl.lock().get());
#else
		TRACE(_T("�ɹ����ӷ����� hdl:%p\r\n"), hdl.lock().get());
#endif // !TRACE
		if (m_pConFunc)
			m_pConFunc(hdl);
	}
	/// <summary>
	/// �����ص�����
	/// </summary>
	/// <param name="c"></param>
	/// <param name="hdl"></param>
	static void on_fail(client* c, websocketpp::connection_hdl hdl) {
		connection_successful = false;
#ifndef TRACE
		printf("<%s>(%d):���ӷ�����ʧ��\r\n", __FILE__, __LINE__);
#else
		TRACE(_T("���ӷ�����ʧ��\r\n"));
#endif // !TRACE
	}
private:
	static websocketpp::connection_hdl  m_hdl;                  // ��������������ӵ�token���ɹ����Ӻ���Ч��
	std::string                         uri;                    // Ҫ���ӵķ�����ws��ַ
	client                              ws_client;              // webSocket�ͻ���
	static bool                         connection_successful;  // �Ƿ����ӳɹ�������
	static conFunc                      m_pConFunc;             // ���ӳɹ���ص����������û�ָ������Ҫ�����Լ�ѭ��
	cRecvMessageFunc                    m_pRMFunc;              // ���յ����ݵĻص�����
};



class WebSocketServer
{
public:
	// ���캯��
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
			// ����������ʧ��
			printf("<%s>(%d):����������ʧ��\r\n", __FILE__, __LINE__);
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
	// ��������
	~WebSocketServer() {
		if (ws_server) {
			// echo_server->close()
			delete ws_server;
			ws_server = NULL;
		}
	}
	// ����������
	int Start() {
		try {
			// ����ָ���Ķ˿�
			ws_server->listen(m_bingPort);
			// �������������ӳ�
			ws_server->start_accept();
			// ����ASIO io��������ѭ��
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
	// ֹͣ������
	int Stop() {
		ws_server->stop();
		return 0;
	}
	// ������Ϣ
	int send(websocketpp::connection_hdl hdl, const std::string& data, websocketpp::frame::opcode::value op) {
		ws_server->send(hdl, data, op);
		return  0;
	}
private:
	/// <summary>
	/// �ͻ������ӳɹ�ʱ�ص�����
	/// </summary>
	/// <param name="hdl"></param>
	static void on_connect(websocketpp::connection_hdl hdl) {
		// server::connection_ptr con = echo_server.get_con_from_hdl(hdl);

		printf("�ͻ��˳ɹ����ӳɹ� hdl:%p\r\n", hdl.lock().get());


		if (m_pConFunc)
			m_pConFunc(hdl); // �û��Զ��崦����
	}
private:
	static server* ws_server;   // webSocket�����
	unsigned short                             m_bingPort;  // webSocket�󶨵Ķ˿�
	static conFunc                             m_pConFunc;  // ���ӳɹ���ص�����
	sRecvMessageFunc                           m_pRMFunc;   // ���յ����ݺ�ص�����
};