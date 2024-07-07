#include "pch.h"
#include "Command.h"

WebSocketClient CCommand::m_wsClient(WS_ADDR, CCommand::ws_on_connect, CCommand::ws_on_message);

CCommand::CCommand(
	DWORD mainId /*= ERROR_INVALID_THREAD_ID*/,
	const std::string& serverIp,
	unsigned short port
)
	: m_serverIp(serverIp)
	, m_serverPort(port)
	, m_imageSendPort(0)
	, m_hCommand(INVALID_HANDLE_VALUE)
	, m_hWsClient(INVALID_HANDLE_VALUE)
	, m_bCommandRun(false)
	, m_hImageSend(INVALID_HANDLE_VALUE)
	, m_bImageSendStop(false)
	, m_mainThreadId(mainId)
	, m_sockSendImage(INVALID_SOCKET)
	, m_loginDlg(mainId, this)
	, m_pUserCfgInfo(NULL)
{
	m_buffer.resize(BUFFER_SIZE);
	// 加载配置文件
	bool ret = m_userCfgInfo.LoadFile(CONFIG_FILE_NAME);
	if (ret == false) {
		// 这个配置文件有问题或者不存在
		// 将这个配置文件重新加载或者重新生成
		USER_CONFIG_INFO uci;
		m_userCfgInfo.AddCfg(uci);
		if (false == m_userCfgInfo.SaveFile(CONFIG_FILE_NAME)) {
			TRACE("file create fail\r\n");
			MessageBoxA(NULL, "文件创建失败，请以管理员权限运行！", "配置错误", MB_OK | MB_USERICON);
			exit(0);
		}
	}
	else {
		m_pUserCfgInfo = m_userCfgInfo.GetUserCfgInfo();
		// 设置登录信息
		m_loginDlg.SetAccText(m_pUserCfgInfo->szUser);
		m_loginDlg.SetPwText(m_pUserCfgInfo->szPwd);
		m_loginDlg.SetRememberPwd(m_pUserCfgInfo->bRememberPwd);
		m_loginDlg.SetAutoLogin(m_pUserCfgInfo->bAutoLogin);
	}
	// 程序初始化进行服务器连接
	if (false == initCmd()) {
		Close();
		exit(0);
	}
	m_wsClient.connect();  // 连接服务器
}

CCommand::~CCommand()
{
	{
		std::string request;
		request += "GET /bectrlDownline HTTP/1.1\r\n";
		request += "Host: 127.0.0.1:9668\r\n";
		request += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.82 Safari/537.36\r\n";
		Json::Value root;
		root["equipName"] = "sb1";
		std::string json = root.toStyledString().c_str();
		root.clear();
		char temp[25] = "";
		snprintf(temp, sizeof(temp), "%lld", json.size());
		request += (std::string("Content-Length: ") + temp + "\r\n");
		request += "Accept: application/json\r\n\r\n";
		request += json;
		m_sock.send(request);
		{
			std::string temp;
			request.swap(temp);
		}
		request.clear();
	}


	Close();
}

void CCommand::Close()
{
	Stop();
	m_serverIp.clear();
	m_buffer.clear();
	m_serverIp.clear();
	m_buffer.clear();
	m_sock.Close();
	m_loginDlg.DestroyWindow();
	m_sysTray.RemoveTray();
}

void CCommand::ws_on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg)
{
	TRACE("Received message:[%s]\r\n", msg->get_payload());
	// 把utf-8编码的转会回来
	std::string data = CTools::from_utf8(msg->get_payload());
	// std::string data = msg->get_payload();
	// std::cout << "(548)Received message: " << data.c_str() << std::endl;

	TRACE("Received message:[%s]\r\n", data.c_str());

	// data是json数据
	// 进行json数据的解析
	Json::Value root;
	Json::Reader reader;
	if (false == reader.parse(data, root)) {
		// 解析失败
		printf("<%s>(%d):[error]json 数据解析失败 data:[%s]\r\n", __FILE__, __LINE__, data.c_str());
		return;
	}
	std::string fields[] = { "opType","dir","auth" };

	if ((false == root.isMember(fields[0]))
		|| (false == root.isMember(fields[1]))
		|| (false == root.isMember(fields[2]))
		) {
		TRACE(_T("数据类型不匹配\r\n"));
		return;
	}
	int type = atoi(root[fields[0]].asString().c_str());
	std::string dir = root[fields[1]].asString();
	std::string toCtrl = root[fields[2]].asString();
	Json::Value retData;
	switch (type)
	{
	case 1:  //  文件预览
		retData = GetFiles(dir, &m_wsClient);
		retData["toCtrl"] = toCtrl;
		retData["opType"] = root[fields[0]].asString();
		retData["equip"] = "sb1";
		m_wsClient.send(retData.toStyledString(), websocketpp::frame::opcode::text);
		break;
	case 2:  //  文件删除
		break;
	case 3: // 文件新建
		break;
	case 4: // 文件执行
		break;
	case 5: // 文件重命名
		break;
	case 6: // 文件上传
		FileUpload(root, &m_wsClient);
		break;
	case 7:  // 文件下载
		FileDownload(root, &m_wsClient);
		break;
	}
}

void CCommand::ws_on_connect(websocketpp::connection_hdl hdl)
{
	// std::string data = "bectrl";
	// std::string data8 = to_utf8(data);

	Json::Value retData;
	retData["equip"] = "sb1";
	retData["opType"] = "0";

	// UTF-8

	// 告诉他你是谁
	// ws_client->send(hdl, data.c_str(), websocketpp::frame::opcode::text);

	m_wsClient.send(retData.toStyledString(), websocketpp::frame::opcode::text);

	TRACE(_T("ws服务器连接成功\n"));
}

Json::Value CCommand::GetFiles(const std::string& dir, WebSocketClient* clien)
{

	Json::Value res; // 要返回的结果
	Json::Value arrFileInfo; // 文件信息数据数组
	Json::Value fItem;   // 每一个文件信息对象
	BOOL ret;   // 返回值
	if (dir == "") {
		// 查看卷
		DWORD len = 0;
		char VolumeName[MAX_PATH] = "";
		HANDLE FindDriHandle = FindFirstVolumeA(VolumeName, sizeof(VolumeName)); // 获取第一个磁盘驱动信息
		if (FindDriHandle == INVALID_HANDLE_VALUE) {
			TRACE("磁盘获取失败！\r\n");
			goto BREAK;
		}

		do {
			// 判断格式是否正确
			size_t index = strlen(VolumeName) - 1;
			if (!VolumeFormat(VolumeName, "\\\\?\\") || VolumeName[index] != '\\') {
				TRACE(_T("磁盘获取失败\r\n"));
				break;
			}
			char driveName[MAX_PATH] = "";
			GetVolumePathNamesForVolumeNameA(VolumeName, driveName, sizeof(driveName), &len); // 获取磁盘名
			if (len > 1) {
				fItem["type"] = "2";
				std::string dir;
				dir += driveName[0];
				fItem["label"] = dir;
				arrFileInfo.append(fItem);
			}
			ZeroMemory(VolumeName, sizeof(VolumeName));
			ret = FindNextVolumeA(FindDriHandle, VolumeName, sizeof(VolumeName));
		} while (ret == TRUE);
		FindVolumeClose(FindDriHandle);
	}
	else {
		// 查看指定目录下的文件
		std::string findDir = dir + '*'; // C://*
		TRACE("dir:[%s]\r\n", findDir);
		WIN32_FIND_DATAA fileInfo = {};
		HANDLE findFileHandle = FindFirstFileA(findDir.c_str(), &fileInfo);
		if (findFileHandle == INVALID_HANDLE_VALUE) {
			// 查找失败
			TRACE("查看指定目录下的文件失败 dir:[%s]\r\n", findDir.c_str());
			goto BREAK;
		}
		do {
			if (strcmp(fileInfo.cFileName, ".") != 0 && (strcmp(fileInfo.cFileName, "..") != 0)) {
				fItem["type"] = (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? "2" : "1";
				fItem["label"] = fileInfo.cFileName;
				arrFileInfo.append(fItem);
			}
			ZeroMemory(&fileInfo, sizeof(fileInfo));
			ret = FindNextFileA(findFileHandle, &fileInfo);
		} while (ret == TRUE);
		FindClose(findFileHandle);
	}
BREAK:
	res["list"] = arrFileInfo;
	return res;
}

BOOL CCommand::VolumeFormat(const char* volumename, const char* format)
{
	for (int i = 0; format[i] != '\0'; i++) {
		if (volumename[i] != format[i])
			return FALSE;
	}
	return TRUE;
}

Json::Value CCommand::FileUpload(const Json::Value& data, WebSocketClient* clien)
{
	/*{
	"auth"   : "...",           // 手机号
	"opType" : "0/1/2/3/4/5/6/7", // 操作类型 上线/文件浏览/删除/新建/运行/重命名/上传/下载
	"EName"  : "...",           // 设备名
	"bNext"  : "0/1",           // 是否有后续  0表示没有/1表示有
	"dir"    : "...",           // 文件绝对路径
	"data"   : "..."            // 文件数据二进制格式
}*/
	Json::Value res; // 要返回的结果

	return res;
}

Json::Value CCommand::FileDownload(const Json::Value& data, WebSocketClient* clien)
{
	/*{
	"auth"   : "...",           // 手机号
	"opType" : "0/1/2/3/4/5/6/7", // 操作类型 上线/文件浏览/删除/新建/运行/重命名/上传/下载
	"EName"  : "...",           // 设备名
	"dir"    : "..."            // 文件绝对路径
}*/
	TRACE("进入到这里\r\n");
	std::string fields[] = { "dir","auth","opType" };
	Json::Value res; // 要返回的结果
	do {
		if (false == data.isMember(fields[0])) {
			// 文件下载不含有路径字段
			TRACE("文件下载不含有路径字段\r\n");
			break;
		}
		if (false == data[fields[0]].isString()) {
			TRACE("数据类型不匹配\r\n");
			break;
		}
		std::string dir = data[fields[0]].asString();
		// 打开文件
		FILE* pFile = fopen(dir.c_str(), "rb");
		// HANDLE hFile = CreateFileA(dir.c_str(), GENERIC_READ, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (pFile == NULL) {
			TRACE("文件打开失败\r\n");
			break;
		}
		// 读取文件大小
		fseek(pFile, 0, SEEK_END);
		long long FileSize = _ftelli64(pFile);
		fseek(pFile, 0, SEEK_SET);
		if (FileSize == -1) {
			TRACE("文件大小获取失败!\r\n");
			break;
		}
		TRACE("文件总大小 %d\r\n", FileSize);

		// 将文件分片发给控制端
		long long totalSize = FileSize;
		char buffer[1024] = "";

		std::string toCtrl = data[fields[1]].asString();
		char ToCtrl[20] = "";

		memcpy(ToCtrl, toCtrl.c_str(), (toCtrl.size() > 19) ? 19 : toCtrl.size());

		size_t RLen = 0;
		do {
			memset(buffer, 0, sizeof(buffer));
			RLen = fread(buffer, 1, sizeof(buffer), pFile);

			TRACE("读出了%d个字节\r\n", RLen);

			/*if (FALSE == ReadFile(hFile, buffer, sizeof(buffer), &RLen, NULL)) {
				TRACE("文件读取失败\n");
				break;
			}*/
			/*res["toCtrl"] = data[fields[1]].asString();
			res["opType"] = data[fields[2]].asString();
			res["equip"] = "sb1";
			res["totalSize"] = (long long)totalSize;
			res["Size"] = (long long)RLen;*/
			std::string Buf(RLen + sizeof(ToCtrl), '\0');
			memcpy((char*)Buf.c_str(), ToCtrl, sizeof(ToCtrl));
			memcpy((char*)Buf.c_str() + sizeof(ToCtrl), buffer, RLen);
			TRACE("一共的字节数 %d\r\n", Buf.size());
			//if (FileSize > RLen) {
			//	// 还有后续
			//	res["bNext"] = 1;
			//	res["data"] = Buf;
			//}
			//else {
			//	// 最后一个包
			//	res["bNext"] = 0;
			//	res["data"] = Buf;
			//}
			FileSize -= RLen;
			// std::string resData = res.toStyledString();
			clien->send(Buf, websocketpp::frame::opcode::binary);
			Sleep(10);
		} while (FileSize > 0);
		// CloseHandle(hFile);
		fclose(pFile);
		res["status"] = "end";
		res["equip"] = "sb1";
		res["opType"] = data[fields[2]].asString();
		res["toCtrl"] = data[fields[1]].asString();
		clien->send(res.toStyledString(), websocketpp::frame::opcode::text);
	} while (false);
	return res;
}

int CCommand::Start(DWORD mainId)
{
	if ((mainId == ERROR_INVALID_THREAD_ID) && (m_mainThreadId == ERROR_INVALID_THREAD_ID)) {
		// 两个都没有值
		return -1;
	}
	if (mainId != ERROR_INVALID_THREAD_ID) {
		// mainId有值
		m_mainThreadId = mainId;
	}
	// 开启之前先把套接字变成非阻塞的，当前状态一定是连接上服务器了并成功登录
	int ret = m_sock.setUnlock(); // 设置为非阻塞套接字
	if (ret != 0)return -1;
	// 这里要变成一个线程
	m_hCommand = (HANDLE)_beginthread(&CCommand::commandThreadEnt, 0, this);
	if (m_hCommand == INVALID_HANDLE_VALUE) {
		TRACE(_T("线程创建失败\n"));
		return -1;
	}

	// 开启webSocket客户端线程
	m_hWsClient = (HANDLE)_beginthread(&CCommand::wsClientThread, 0, this);
	if (m_hWsClient == INVALID_HANDLE_VALUE) {
		// 这里应该把上面创建成功的线程关闭
		Stop();
		TRACE(_T("线程创建失败！\n"));
		return -1;
	}
	m_bCommandRun = true;
	return 0;
}

int CCommand::Connect()
{
	if ((m_serverIp == "") || (m_serverPort <= 0)) {
		return -1;
	}
	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(m_serverIp.c_str());
	serverAddr.sin_port = htons(m_serverPort);
	if (0 == m_sock.connect(serverAddr)) {
		// 服务器连接成功
		return 0;
	}
	return -1;
}

int CCommand::Stop()
{
	// 如果屏幕监控开启，则先关闭该线程
	StopImageSend();
	if ((m_hCommand != INVALID_HANDLE_VALUE) && (m_bCommandRun == true)) {
		m_bCommandRun = false;
		HANDLE hCommand = m_hCommand;
		m_hCommand = INVALID_HANDLE_VALUE;
		if (WAIT_TIMEOUT == WaitForSingleObject(hCommand, INFINITE)) {
			// 线程结束等待超时，就要强制将线程结束掉
			TRACE(_T("线程没自动关闭，已被强制关闭\n"));
			TerminateThread(hCommand, 1);
		}
	}
	// 关闭ws客户端线程
	if (m_hWsClient != INVALID_HANDLE_VALUE) {
		HANDLE hWsClient = m_hWsClient;
		m_hWsClient = INVALID_HANDLE_VALUE;
		m_wsClient.Stop();
		if (WAIT_TIMEOUT == WaitForSingleObject(hWsClient, 500)) {
			// 线程结束等待超时，就要强制将线程结束掉
			TRACE(_T("线程没自动关闭，已被强制关闭\n"));
			TerminateThread(hWsClient, 1);
		}
	}
	return 0;
}

void CCommand::setMainThreadId(DWORD mainId)
{
	m_mainThreadId = mainId;
}

bool CCommand::initCmd()
{
	// 首先连接服务器
	int ret = Connect();
	if (ret < 0) {
		MessageBox(NULL, _T("当前服务器不在线，请稍后访问\n请检测网络是否连接"), _T("服务器错误"), MB_OK | MB_USERICON);
		return false;
	}
	// 然后让用户进行登录
	// 发送一个http数据表示我是被控端
	if (login() == false) {
		return false;
	}
	if (false == initSysTray()) {
		return false;
	}
	return true;
}

void CCommand::commandThreadEnt(void* arg)
{
	CCommand* thiz = (CCommand*)arg;
	thiz->commandThreadMain();
	_endthread();
}

void CCommand::commandThreadMain()
{
	char* pData = (char*)m_buffer.c_str();
	long long index = 0;
	// static long long mtotal = 0;
	while (m_bCommandRun) {
		long long len = m_sock.recv(pData + index, (int)(BUFFER_SIZE - index), 0);
		if (len <= 0) {
			int Errno = WSAGetLastError();
			// TRACE("len=%d recv total len %lld  %d\r\n", len, index, Errno);
			if (WSAEWOULDBLOCK == Errno) {
				if (index > 0) {
					// 证明里面还有数据
					len = 0;
				}
				else {
					Sleep(50);
					continue;
				}
			}
			else {
				TRACE(_T("调用到这里\n"));
				break;
			}
		}
		index += len;
		len = index;
		CPacket pack(pData, len);
		if (len == 0) {
			continue;
		}
		if (len > 0) {
			// 解析成功
			memmove(pData, pData + len, size_t(BUFFER_SIZE - len));
			TRACE("解析成功 cmd:%d\r\n", pack.GetCmd());
			switch (pack.GetCmd())
			{
			case CTC_STARTEQUIPCTRL: {// 监控屏幕
				// 这里数据包里面的m_num就是端口号
				m_imageSendPort = pack.GetNum();
				m_imageSendName = pack.GetData();
				TRACE(_T("获取到发送端口：%d  %s\n"), m_imageSendPort, m_imageSendName.c_str());
				StartImageSend();
				break;
			}
			case CTC_ENDEQUIPCTRL: {// 停止监控
				// printf("%s(%d):已停止屏幕信息发送\n", __FILE__, __LINE__);
				// TRACE(_T("mouse total %lld\n"), mtotal);
				StopImageSend();
				break;
			}
			case CTC_CTRLEVENT: { // 鼠标键盘消息
				// data里面是鼠标键盘数据
				MKEvent ME;
				memcpy(&ME, pack.GetData(), sizeof(ME));
				MKEventDispose(ME);
				// printf("鼠标键盘事件处理完成:[x:%d, y:%d]\n", ME.MouseXY.x, ME.MouseXY.y);
				// mtotal++;
				break;
			}
			case CTC_EQUIPSHUTDOWN: { // 设备关机操作
				TRACE("要求关机的时间 %s\r\n", pack.GetData());   // TODO: 将数据转换为小数类型
				const char* message = "计算机将在1分钟后关闭，请保存您的工作并退出程序\n\
软件右键托盘撤销关机！！\n\
			——————CTCtrl开发团队";
				CTools::LocalSystemShutdown(message);

				Sleep(5000);

				// 取消事件
				CTools::RevocationLocalSystemShutdown();
				TRACE("操作已撤销 %s\r\n", pack.GetData());
				break;
			}
			case CTC_EQUIPRESTART: {  // 设备重启操作
				TRACE("要求重启的时间 %s\r\n", pack.GetData());
				const char* message = "计算机将在1分钟后重启，请保存您的工作并退出程序\n\
软件右键托盘撤销重启！！\n\
			——————CTCtrl开发团队";
				CTools::LocalSystemShutdown(message, true);

				Sleep(5000);

				// 取消事件
				CTools::RevocationLocalSystemShutdown();
				TRACE("操作已撤销 %s\r\n", pack.GetData());
				break;
			}
			}
		}
		else {
			// 数据包校验和失败
			len = -1 * len;
			memmove(pData, pData + len, size_t(BUFFER_SIZE - len));
		}
		index -= len;
	}
	// TRACE(_T("线程退出 %d %ld\n"), WSAGetLastError(), WSAEWOULDBLOCK);
	PostThreadMessage(m_mainThreadId, CLOSEAPP, 0, 0);
	// PostThreadMessage(m_mainThreadId, CLOSEAPP, 0, 0);
}

void CCommand::wsClientThread(void* arg)
{
	m_wsClient.Start();
	_endthread();
}

void CCommand::imageSendThreadEnt(void* arg)
{
	CCommand* thiz = (CCommand*)arg;
	thiz->imageSendThreadMain();
	_endthread();
}

void CCommand::imageSendThreadMain()
{
	if (m_imageSendPort <= 0 || m_imageSendName == "") {
		return;
	}
	m_sockSendImage = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockSendImage == INVALID_SOCKET) {
		TRACE(_T("套接字创建失败\n"));
		return;
	}
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(m_serverIp.c_str());
	addr.sin_port = htons(m_imageSendPort);
	if (connect(m_sockSendImage, (sockaddr*)&addr, sizeof(addr)) != 0) {
		TRACE(_T("连接失败\n"));
		closesocket(m_sockSendImage);
		m_sockSendImage = INVALID_SOCKET;
		return;
	}

	// IMAGEDATA   ImageData(m_imageSendName.c_str(), m_imageSendName.size());
	std::string imageData;
	int num = 1;
	while (m_bImageSendStop == true) {
		// ImageData.m_data.clear();
		imageData.clear();
		HDC hDC = GetDC(NULL);
		cv::Rect recv(0, 0, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES));
		ReleaseDC(WindowFromDC(hDC), hDC);
		// CTools::screenCapture(ImageData.m_data, recv);
		CTools::screenCapture(imageData, recv);
		// TRACE(_T("当前图片大小为 %lld\n"), ImageData.m_data.size());

		TRACE(_T("当前图片大小为 %lld\n"), imageData.size());

		// 图片名
		// 图片数据
		// CPacket pack(CTC_STARTEQUIPCTRL, num, ImageData.toBytes(), ImageData.Size());
		CPacket pack(CTC_STARTEQUIPCTRL, num, imageData.c_str(), imageData.size());
		long long len = pack.Size();
		const char* pData = pack.outStr();
		int ret = send(m_sockSendImage, pData, (int)len, 0);
		if (ret < 0) {
			break;
		}
		::Sleep(1);
		// printf("send len=%d\n", ret);
		num++;
	}
	TRACE(_T("%s(%d):停止屏幕信息发送\n"), __FILE__, __LINE__);
	closesocket(m_sockSendImage);
	m_sockSendImage = INVALID_SOCKET;
}

int CCommand::MKEventDispose(const MKEvent& mke)
{
	INPUT input{};
	memset(&input, 0, sizeof(0));
	if (mke.EType == 1) {
		// 这个是鼠标事件
		// 先移动鼠标到指定位置
		if ((mke.MouseXY.x < 0) || (mke.MouseXY.y < 0)) {
			return -1;
		}
		// 设置鼠标位置
		SetCursorPos(mke.MouseXY.x, mke.MouseXY.y);
		input.type = INPUT_MOUSE;
		switch (mke.MKOperation)
		{
		case 0X11: // 右键按下
			input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			// printf("%s(%d):鼠标右键按下\n", __FILE__, __LINE__);
			break;
		case 0X12: // 中键按下
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			// printf("%s(%d):鼠标中键按下\n", __FILE__, __LINE__);
			break;
		case 0X14: // 左键按下
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			// printf("%s(%d):鼠标左键按下\n", __FILE__, __LINE__);
			break;
		case 0X21: // 右键抬起
			input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			// printf("%s(%d):鼠标右键抬起\n", __FILE__, __LINE__);
			break;
		case 0X22: // 中键抬起
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			// printf("%s(%d):鼠标中键抬起\n", __FILE__, __LINE__);
			break;
		case 0X24: // 左键抬起
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			// printf("%s(%d):鼠标左键抬起\n", __FILE__, __LINE__);
			break;
		case 0X40: // 滚轮滚动
			input.mi.dwFlags = MOUSEEVENTF_WHEEL;
			input.mi.mouseData = mke.MKKeyValue * (-1);
			TRACE(_T("%s(%d):鼠标滚动抬起 %ld\n"), __FILE__, __LINE__, input.mi.mouseData);
			break;
		default:
			goto BREAK;
			break;
		}
		input.mi.dwExtraInfo = GetMessageExtraInfo();
	}
	else if (mke.EType == 2) {
		input.type = INPUT_KEYBOARD;
		// 按下不用设置ki.dwFlags
		if (mke.MKOperation & 0X20) { // 抬起事件
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			TRACE(_T("%s(%d):键盘抬起\n"), __FILE__, __LINE__);
			// WM_KEYUP;
		}
		input.ki.wVk = (WORD)mke.MKKeyValue;
		input.ki.dwExtraInfo = GetMessageExtraInfo();
	}
	else {
		goto BREAK;
	}
	SendInput(1, &input, sizeof(INPUT));
	// VK_LWIN  91
BREAK:
	return 0;
}

int CCommand::StartImageSend()
{
	if (m_bImageSendStop == true) {
		// 不能再次开启
		return -1;
	}
	m_hImageSend = (HANDLE)_beginthread(&CCommand::imageSendThreadEnt, 0, this);
	if (m_hImageSend != INVALID_HANDLE_VALUE) {
		m_bImageSendStop = true;
		return 1;
	}
	return -2;
}

void CCommand::StopImageSend()
{
	if (m_bImageSendStop == true) {
		m_bImageSendStop = false;
		if (m_hImageSend != INVALID_HANDLE_VALUE) {
			if (WAIT_TIMEOUT == WaitForSingleObject(m_hImageSend, 10000)) {
				// 线程结束等待超时，就要强制将线程结束掉
				TerminateThread(m_hImageSend, 1);
				if (m_sockSendImage != INVALID_SOCKET) {
					closesocket(m_sockSendImage);
					m_sockSendImage = INVALID_SOCKET;
				}
			}
			m_hImageSend = INVALID_HANDLE_VALUE;
		}
	}
}

bool CCommand::login()
{
	do {
		// 先进行登录操作
		INT_PTR loginRet = m_loginDlg.DoModal();
		TRACE("%d, %d\n", m_loginDlg.GetRememberPwd(), m_loginDlg.GetAutoLogin());
		if (loginRet == IDCANCEL) {
			// 用户点击了取消或者×

			break;
		}
		// m_mainDlg.DestroyWindow();
		std::string phone = CTools::wcharStr2charStr(m_loginDlg.GetAccText()).c_str();
		std::string password = CTools::wcharStr2charStr(m_loginDlg.GetPwText()).c_str();
		{
			std::string request;
			request += "GET /bectrlOnline HTTP/1.1\r\n";
			request += "Host: 192.168.1.6:9668\r\n";
			request += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.82 Safari/537.36\r\n";
			Json::Value root;
			root["equipName"] = "sb1";
			HDC hDC = GetDC(NULL);
			root["width"] = GetDeviceCaps(hDC, HORZRES);
			root["height"] = GetDeviceCaps(hDC, VERTRES);
			ReleaseDC(WindowFromDC(hDC), hDC);
			root["phone"] = phone;
			root["password"] = password;
			root["type"] = "1";
			std::string json = root.toStyledString().c_str();
			root.clear();
			char temp[25] = "";
			snprintf(temp, sizeof(temp), "%lld", json.size());
			request += (std::string("Content-Length: ") + temp + "\r\n");
			request += "Accept: application/json\r\n\r\n";
			request += json;
			m_sock.send(request);
			{
				std::string temp;
				request.swap(temp);
			}
			request.clear();
		}
		char buffer[10240] = "";
		long long len = m_sock.recv(buffer, sizeof(buffer), 0);
		// 
		TRACE(_T("[info]:recv data len:%lld\n"), len);
		// printf("[info]:recv data len:%lld ret:%d\n", len, ret);

		// 对数据进行解析，判断是否登录成功
		CHttpParser parser(HTTP_RESPONSE);
		size_t size = parser.Parser(buffer);
		if (size == 0 || (parser.Errno() != 0)) {
			TRACE(_T("[error]:size %llu errno:%d\n"), size, parser.Errno());
			break;
		}
		std::string buf = parser.Body();
		Json::Value rootret;
		Json::Reader reader;
		if (false == reader.parse(buf, rootret)) {
			TRACE(_T("数据解析失败\n"));
			break;
		}
		if (false == rootret.isMember("status")) {
			// 数据不匹配
			break;
		}
		if (true == rootret["status"].isArray()) {
			// 不能为数组值
			break;
		}
		int status = rootret["status"].asInt();
		if (status != 0) {
			// 登录失败
			std::wstring firstLine = L"通知";
			std::wstring secondLine = L"登录失败,请重新登录\n如忘记密码，请到网页端找回";
			CToast::ShowNotification(firstLine, secondLine);
			continue;
		}
		else {
			std::wstring firstLine = L"通知";
			std::wstring secondLine = L"登录成功";
			CToast::ShowNotification(firstLine, secondLine);
			// 把登录配置保存到文件中
			m_pUserCfgInfo = m_userCfgInfo.GetUserCfgInfo();
			memcpy(m_pUserCfgInfo->szUser, phone.c_str(), phone.size());
			memcpy(m_pUserCfgInfo->szPwd, password.c_str(), password.size());
			m_pUserCfgInfo->bRememberPwd = m_loginDlg.GetRememberPwd() == TRUE ? true : false;
			m_pUserCfgInfo->bAutoLogin = m_loginDlg.GetAutoLogin() == TRUE ? true : false;
			m_userCfgInfo.ModifyCfg(*m_pUserCfgInfo);
			m_userCfgInfo.SaveFile(CONFIG_FILE_NAME);
			return true;
		}
	} while (true);
	return false;
}

bool CCommand::initSysTray()
{
	m_loginDlg.Create(IDD_DLG_LOGIN);
	HICON hIcon = CTools::LoadIconRes((CTools::GetAppPathW() + _T("image\\logo.ico")));
	if (hIcon == NULL) {
		// 图标资源加载失败
		TRACE(_T("图标资源加载失败 %d\n"), GetLastError());
		return false;
	}
	BOOL ret = m_sysTray.AddTray(m_loginDlg.GetSafeHwnd(), WM_TRAYICON_NOTIFY, 1, hIcon, _T("beCtrl-空闲"));
	if (ret == FALSE) {
		// 系统托盘创建失败
		TRACE(_T("%s(%d):系统托盘创建失败 %d\n"), __FILE__, __LINE__, GetLastError());
		m_loginDlg.DestroyWindow();
		CTools::DestroyIconRes(hIcon);
		return false;
	}
	CTools::DestroyIconRes(hIcon);
	return true;
}
