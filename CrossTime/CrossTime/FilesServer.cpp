#include "FilesServer.h"

CFilesServer::CFilesServer(int TPool_cnt, int sock_lis_cnt, short sock_port, const std::string& sock_bind_ip)
	: m_ThreadPool(TPool_cnt)
	, m_ep(1)
	, m_serverSock(-1)
	, m_listenCnt(sock_lis_cnt)
	, m_serverPort(sock_port)
	, m_serverIp(sock_bind_ip)
{
	bool ret = m_ThreadPool.Invoke();
	if (ret == false) {

		LOG_ERROR << "线程池启动失败";
		CCrossTimeServer::m_logFile->flush();
		exit(0);
	}
	// 初始化服务器套接字
	if (sockInit() < 0) {
		// 套接字初始化失败
		LOG_ERROR << "套接字初始化失败";
		CCrossTimeServer::m_logFile->flush();
		exit(0);
	}
	// 函数映射表初始化
	initFuncMapping();
}

CFilesServer::~CFilesServer()
{
	m_ThreadPool.Stop();
	socketClose();
}



int CFilesServer::sockInit()
{
	if (m_serverSock > 0) {
		// 已经存在连接的套接字了
		int temp = m_serverSock;
		m_serverSock = -1;
		close(temp);
	}

	// 套接字创建
	m_serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_serverSock < 0) {
		LOG_ERROR << "套接字创建失败";
		CCrossTimeServer::m_logFile->flush();
		return -1;
	}
	// 设置允许端口复用
	int optval = 1;
	if (setsockopt(
		m_serverSock,
		SOL_SOCKET,
		SO_REUSEADDR,
		&optval,
		sizeof(optval)) < 0)
	{
		LOG_ERROR << "重用端口设备失败";
		CCrossTimeServer::m_logFile->flush();
		socketClose();
		return false;
	}
	// 绑定端口
	sockaddr_in serAddr = {};
	serAddr.sin_family = PF_INET;
	serAddr.sin_port = htons(m_serverPort);
	serAddr.sin_addr.s_addr = inet_addr(m_serverIp.c_str());
	if (bind(
		m_serverSock,
		(sockaddr*)&serAddr,
		sizeof(serAddr)) < 0)
	{
		// 端口绑定失败

		LOG_ERROR << "服务器端口绑定失败";
		CCrossTimeServer::m_logFile->flush();
		socketClose();
		return -1;
	}
	// 监听端口
	if (listen(m_serverSock, m_listenCnt) < 0) {
		// 套接字监听失败
		LOG_ERROR << "服务器套接字监听失败";
		CCrossTimeServer::m_logFile->flush();
		socketClose();
		return -1;
	}
	return 0;
}

void CFilesServer::socketClose()
{
	if (m_serverSock > 0) {
		// 已经存在连接的套接字了
		int temp = m_serverSock;
		m_serverSock = -1;
		close(temp);
	}
}

void CFilesServer::initFuncMapping()
{
	struct dis2func
	{
		std::string dis;
		DIS_FUNC    func;
	};
	// GET请求映射
	dis2func GETfd[] = {
		{ "getImage", &CFilesServer::getImageDispose }
	};
	// POST请求映射

	// DELETE请求映射

	// GET请求注册
	for (size_t i = 0; i < sizeof(GETfd) / sizeof(*GETfd); ++i) {
		m_mGETUriDisfunc.insert(std::pair<std::string, DIS_FUNC>(GETfd[i].dis, GETfd[i].func));
	}
	// POST请求注册

	// DELETE请求注册

}

int CFilesServer::start()
{
	// 将服务器套接字添加到epoll中
	epoll_event ev = {};
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = m_serverSock;
	if (m_ep.Add_fd(m_serverSock, &ev) < 0) {

		LOG_ERROR << "文件描述符添加失败";
		CCrossTimeServer::m_logFile->flush();
		return -1;
	}
	Dispose();
	return 0;
}

int CFilesServer::Dispose()
{
	// 向线程池投递任务
	size_t cnt = m_ThreadPool.ThreadCnt();

	for (int i = 0; i < (int)cnt; i++) {
		ThreadWorker worker(&CFilesServer::MyWorker, this);
		m_ThreadPool.DispatchWorker(worker);
	}
	return 0;
}

int CFilesServer::MyWorker(void* argc)
{
	CFilesServer* thiz = (CFilesServer*)argc;
	thiz->mainWorker();
	return 0;
}

void CFilesServer::mainWorker()
{
	epoll_event evlist[252] = {};
	epoll_event ev = {};
	while (1) {
		int len = m_ep.EWait(evlist, 252, -1);
		if (len > 0) {
			// 有文件描述符事件触发
			for (int i = 0; i < len; i++) {
				int fd = evlist[i].data.fd;
				if (fd == m_serverSock) {
					// 客户端连接
					sockaddr_in cliAddr = {};
					socklen_t addrLen = sizeof(sockaddr_in);
					int sock = accept(m_serverSock, (sockaddr*)&cliAddr, &addrLen);
					if (sock < 0) {
						// 客户端连接失败

						LOG_ERROR << "客户端连接失败";
						CCrossTimeServer::m_logFile->flush();
						continue;
					}
					// 有客户端连接
					FCLIENTINFO* cliInfo = new FileClientInfo(sock, cliAddr);
					if (cliInfo == NULL) {
						// 没有空间了
						close(sock);
						break;
					}
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = sock;
					setNoblock(cliInfo->m_sock);  // 把客户端套接字设置成非阻塞的
					int ret = m_ep.Add_fd(cliInfo->m_sock, &ev);
					if (ret < 0) {
						// 添加失败
						delete cliInfo;
						close(sock);
					}
					auto it = m_mapClient.find(sock);
					if (it != m_mapClient.end()) {
						// 有套接字
						int sock = it->first;
						FCLIENTINFO* cInfo = it->second;

						close(sock);
						delete cInfo;
					}
					else {
						m_mapClient.insert(std::pair<int, FCLIENTINFO*>(sock, cliInfo));
						LOG_INFO << "客户端连接成功 sock:" << sock;
						CCrossTimeServer::m_logFile->flush();
					}
				}
				else {
					// 有客户端发送消息
					// int sock = evlist[i].data.fd;

					auto it = m_mapClient.find(fd);
					if (it == m_mapClient.end()) {
						continue;
					}
					FCLIENTINFO* cInfo = it->second;
					if (cInfo == NULL)
						continue;

					char* buf = (char*)cInfo->m_buf.c_str();
					size_t size = cInfo->m_buf.size();
					long long len = 0;
					while (1) {
						{
							std::unique_lock<std::mutex> lock(cInfo->m_lock);
							len = read(fd, buf + cInfo->m_index, size - cInfo->m_index);
							if (len > 0)cInfo->m_index += len;
						}
						if (len <= 0) {
							if (len == 0) {
								// 套接字关闭了
								// 客户端端口连接

								LOG_INFO << "客户端断开了";
								CCrossTimeServer::m_logFile->flush();
								m_mapClient.erase(fd);
								m_ep.Del_fd(fd);
								close(fd);
								delete cInfo;
								break;
							}
							if (len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
								break;
							}
						}
						len = cInfo->m_index;
						// TODO： 没有数据或者说解压出一个包之后就可以直接返回了
						// 首先进行http协议解析
						CHttpParser parser;
						size_t httpSize = parser.Parser(buf, len);
						if ((httpSize > 0) && (parser.Errno() == 0)) {
							// httpSize返回的还是解析的位置
							{
								std::unique_lock<std::mutex> lock(cInfo->m_lock);
								memmove(buf, buf + httpSize, size - httpSize);
								cInfo->m_index -= httpSize;
							}

							LOG_INFO << "数据解析成功 http";
							CCrossTimeServer::m_logFile->flush();

							std::string header;
							std::string bodyData;

							int ret = HttpParser(parser, header, bodyData, fd);
							if (ret < 0) {
								// 处理失败
								LOG_DEBUG << "请求处理失败 data:[" << buf << "]";
								CCrossTimeServer::m_logFile->flush();

							}


							Buffer response = MakeAckPacket(header, bodyData);

							LOG_INFO << "要给客户端发送的数据为 data:[" << response.c_str() << "]";
							CCrossTimeServer::m_logFile->flush();

							len = send(fd, response.c_str(), response.size(), 0);
							if (len <= 0) {
								LOG_INFO << "客户端断开了";
								CCrossTimeServer::m_logFile->flush();
								m_mapClient.erase(fd);
								m_ep.Del_fd(fd);
								close(fd);
								{
									std::unique_lock<std::mutex> lock(m_ImageDatalock);
									m_mImageData.erase(cInfo->m_imageName);
								}
								delete cInfo;
							}
							break;
						}
						else {
							// 这个http解析不了，跳到这里面了，看看是不是自定义数据包进行解析
							CPacket pack(buf, len);
							if (len > 0) {
								// 数据解析成功
								{
									std::unique_lock<std::mutex> lock(cInfo->m_lock);
									memmove(buf, buf + len, size - len);
									cInfo->m_index -= len;
								}

								switch (pack.GetCmd()) {
								case 1:  // 监控画面数据

									// 吧这个图片解析保存到目标文件中
									IMAGEDATA ImageData;
									bool ret = ImageData.parse(pack.GetData(), pack.GetDataSize());
									if (ret) {
										// 把这个数据保存到图片数据映射表里面

										{
											std::unique_lock<std::mutex> lock(m_ImageDatalock);

											std::map<std::string, std::string>::iterator it = m_mImageData.find(ImageData.m_name);
											if (it == m_mImageData.end()) {
												// 没有这个东西
												// std::string data;
												// data.resize(ImageData.m_data.size());
												// memcpy((char*)data.c_str(), ImageData.m_data.c_str(), ImageData.m_data.size());
												LOG_INFO << "数据已保存到内存中 name:[" << ImageData.m_name << "]";
												CCrossTimeServer::m_logFile->flush();
												m_mImageData.insert(std::pair<std::string, std::string>(ImageData.m_name, ImageData.m_data));
											}
											else {
												// 有这个东西
												LOG_INFO << "数据已保存到内存中 name:[" << ImageData.m_name << "]";
												CCrossTimeServer::m_logFile->flush();
												it->second.resize(ImageData.m_data.size());
												char* pData = (char*)it->second.c_str();
												memcpy(pData, ImageData.m_data.c_str(), it->second.size());
											}
											cInfo->m_imageName = ImageData.m_name;
										}

										/*std::string path = "/var/www/example/img/" + (std::string)ImageData.m_name + ".jpeg";

										int fd = open(path.c_str(), O_RDWR | O_CREAT, 0666);
										if (fd < 0) {
											LOG_ERROR << "文件创建失败！[" << path.c_str() << "]";
											CCrossTimeServer::m_logFile->flush();
											break;
										}
										ssize_t ret = write(fd, ImageData.m_data.c_str(), ImageData.m_data.size());
										LOG_INFO << "图片写入字节数 ret=" << ret << " 图片位置:[" << path << "]";
										CCrossTimeServer::m_logFile->flush();
										if (ret < 0) {
											LOG_ERROR << "写入错误 ret = " << ret;
											CCrossTimeServer::m_logFile->flush();
										}
										close(fd);*/
									}
									break;
								}
							}
							else if (len < 0) {
								// 数据包校验和失败
								len = -1 * len;
								{
									std::unique_lock<std::mutex> lock(cInfo->m_lock);
									memmove(buf, buf + len, size - len);
									cInfo->m_index -= len;
								}
								break;
							}
						}
					}
				}
			}
		}
		else if (len < 0) {
			LOG_ERROR << "len小于0 len:" << len << " errno:" << errno << " " << strerror(errno);
			CCrossTimeServer::m_logFile->flush();
			break;
		}
	}
}

int CFilesServer::setNoblock(int fd)
{
	int oldfl = fcntl(fd, F_GETFL);
	int newfl = oldfl | O_NONBLOCK;
	return fcntl(fd, F_SETFL, newfl);
}

int CFilesServer::HttpParser(const CHttpParser& parser, std::string& header, std::string& bodyData, SOCKET clientSocket)
{
	// 没有错误并且是http协议
	UrlParser url("http://127.0.0.1" + parser.Url());
	int ret = url.Parser();
	if (ret != 0) {
		LOG_ERROR << "ret=" << ret << " url:[" << ("http://127.0.0.1" + parser.Url()) << "]";
		CCrossTimeServer::m_logFile->flush();
		return -1;
	}
	else {
		Buffer uri = url.Uri();

		LOG_INFO << "uri:[" << uri << "]";
		CCrossTimeServer::m_logFile->flush();

		switch (parser.Method()) {
		case HTTP_GET:
			auto it = m_mGETUriDisfunc.find(uri);
			if (it != m_mGETUriDisfunc.end()) {
				return (this->*it->second)(url, parser, header, bodyData, clientSocket);
			}
			else {
				// 没有找到
				LOG_ERROR << "没有这个uri:[" << uri.c_str() << "]";
				CCrossTimeServer::m_logFile->flush();
			}
			break;
		}
	}


	return 0;
}

Buffer CFilesServer::MakeAckPacket(std::string& header, std::string& bodyData)
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
	result += "Server: CrossTimeServer/1.0\r\n";

	// 解决跨域问题
	result += "Access-Control-Allow-Origin: *\r\n";
	result += "Access-Control-Allow-Methods: GET,  POST, PUT, OPTIONS, DELETE\r\n";
	result += "Access-Control-Allow-Headers: X-Requested-With\r\n";
	result += "Connection: Keep-Alive\r\n";  // Close
	result += "Cache-Control: no-cache\r\n";

	if (header.size() > 0) {
		result += header.c_str();
	}

	if (bodyData.size() > 0) {
		// 有body数据
		memset(temp, 0, sizeof(temp));
		snprintf(temp, sizeof(temp), "%d", bodyData.size());
		result += (Buffer("Content-Length: ") + temp + "\r\n");
	}



	result += "\r\n";
	if (bodyData.size() > 0) {
		result += bodyData;
	}
	return result;
}

int CFilesServer::getImageDispose(const UrlParser& url, const CHttpParser& parser, std::string& header, std::string& bodyData, SOCKET clientSocket)
{
	// 先获取文件名
	Buffer imageName = url["name"];
	LOG_INFO << "image get: name [" << imageName << "]";
	CCrossTimeServer::m_logFile->flush();
	{
		std::unique_lock<std::mutex> lock(m_ImageDatalock);
		auto it = m_mImageData.find(imageName);
		if (it != m_mImageData.end()) {
			bodyData = it->second;
		}
	}
	header = "Content-Type: image/jpeg\r\nAccept-Ranges: bytes\r\n";
	return 0;
}
