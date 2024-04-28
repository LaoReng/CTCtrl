#include "pch.h"
#include "Command.h"

CCommand::CCommand(DWORD mainId /*= ERROR_INVALID_THREAD_ID*/)
	: m_serverPort(0), m_imageSendPort(0)
	, m_hCommand(INVALID_HANDLE_VALUE)
	, m_bCommandRun(false)
	, m_hImageSend(INVALID_HANDLE_VALUE)
	, m_bImageSendStop(false)
	, m_mainThreadId(mainId)
	, m_sockSendImage(INVALID_SOCKET)
	, m_mainDlg(mainId)
{
	m_bCommandRun = false;
	m_bImageSendStop = false;
	m_buffer.resize(BUFFER_SIZE);
	initCmd();
}

void CCommand::Close()
{
	{
		std::string temp;
		m_serverIp.swap(temp);
	}
	{
		std::string temp;
		m_buffer.swap(temp);
	}
	m_serverIp.clear();
	m_buffer.clear();
	m_sock.Close();
	m_mainDlg.DestroyWindow();
	m_sysTray.RemoveTray();
}

CCommand::~CCommand()
{
	Close();
	Stop();
}

CCommand::CCommand(DWORD mainId /*= ERROR_INVALID_THREAD_ID*/, const std::string& serverIp, unsigned short port)
	: m_serverIp(serverIp), m_serverPort(port), m_imageSendPort(0)
	, m_hCommand(INVALID_HANDLE_VALUE)
	, m_bCommandRun(false)
	, m_hImageSend(INVALID_HANDLE_VALUE)
	, m_bImageSendStop(false)
	, m_mainThreadId(mainId)
	, m_mainDlg(mainId)
{
	m_buffer.resize(BUFFER_SIZE);
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
		TRACE(_T("%s(%d):线程创建失败\n"), __FILE__, __LINE__);
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
			TerminateThread(hCommand, 1);
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
			switch (pack.GetCmd())
			{
			case 1: // 监控屏幕
			// 这里数据包里面的m_num就是端口号
				m_imageSendPort = pack.GetNum();
				// printf("获取到发送端口：%d\n", m_imageSendPort);
				StartImageSend();
				break;
			case 2: // 停止监控
				// printf("%s(%d):已停止屏幕信息发送\n", __FILE__, __LINE__);
				// TRACE(_T("mouse total %lld\n"), mtotal);
				StopImageSend();
				break;
			case 3: // 鼠标键盘消息
				// data里面是鼠标键盘数据
				MKEvent ME;
				memcpy(&ME, pack.GetData(), sizeof(ME));
				MKEventDispose(ME);
				// printf("鼠标键盘事件处理完成:[x:%d, y:%d]\n", ME.MouseXY.x, ME.MouseXY.y);
				// mtotal++;
				break;
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

void CCommand::imageSendThreadEnt(void* arg)
{
	CCommand* thiz = (CCommand*)arg;
	thiz->imageSendThreadMain();
	_endthread();
}

void CCommand::imageSendThreadMain()
{
	if (m_imageSendPort <= 0) {
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
	std::string imageData;
	int num = 1;
	while (m_bImageSendStop == true) {
		imageData.clear();
		HDC hDC = GetDC(NULL);
		cv::Rect recv(0, 0, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES));
		ReleaseDC(WindowFromDC(hDC), hDC);
		SaveScreen(imageData, recv);
		TRACE(_T("当前图片大小为 %lld\n"), imageData.size());
		CPacket pack(1, num, imageData.c_str(), imageData.size());
		long long len = pack.Size();
		const char* pData = pack.outStr();
		int ret = send(m_sockSendImage, pData, (int)len, 0);
		if (ret < 0) {
			break;
		}
		::Sleep(10);
		// printf("send len=%d\n", ret);
		num++;
	}
	TRACE(_T("%s(%d):停止屏幕信息发送\n"), __FILE__, __LINE__);
	closesocket(m_sockSendImage);
	m_sockSendImage = INVALID_SOCKET;
}

int CCommand::SaveScreen(std::string& imageData, cv::Rect& recv)
{
	// 利用CImage类实现屏幕截图
	HDC hDC = GetDC(NULL);
	// 在创建时，创建指定尺寸的Image
	CImage image;
	if (image.Create(recv.width, recv.height, GetDeviceCaps(hDC, BITSPIXEL)) == FALSE) {
		TRACE(_T("图片创建失败\r\n"));
		return -1;
	}
	// 复制指定区域的屏幕，到Image中
	if (BitBlt(image.GetDC(), 0, 0, image.GetWidth(), image.GetHeight(), hDC, 0, 0, SRCCOPY) == FALSE) {
		TRACE(_T("图片复制失败\r\n"));
		return -2;
	}
	ReleaseDC(WindowFromDC(hDC), hDC);

	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (hMem == NULL) {
		return -3;
	}
	IStream* pStream = NULL;
	HRESULT ret = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if (ret != S_OK) {
		TRACE(_T("内存绑定创建失败\r\n"));
		return -4;
	}



	image.Save(pStream, Gdiplus::ImageFormatJPEG);
	LARGE_INTEGER bg = {};
	pStream->Seek(bg, STREAM_SEEK_SET, NULL);
	PBYTE pData = (PBYTE)GlobalLock(hMem);
	if (pData != NULL) {
		SIZE_T nSize = GlobalSize(hMem);

		do {
			std::string data(nSize, '\0');
			memcpy((char*)data.c_str(), pData, nSize);
			char* pData = (char*)data.c_str();
			int imageSize = (int)data.size();
			cv::_InputArray src_arr(pData, imageSize);
			cv::Mat src_mat = cv::imdecode(src_arr, 1);
			{
				std::string str;
				data.swap(str);
			}
			cv::Mat dst_src = src_mat(recv);
			std::vector<uchar> pic_buff;
			std::vector<int> compression_params;
			compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
			compression_params.push_back(20);
			bool ret = cv::imencode(".jpg", dst_src, pic_buff, compression_params);
			if (ret == false) {
				break;
			}
			imageData.resize(pic_buff.size());
			memcpy((char*)imageData.c_str(), pic_buff.data(), imageData.size());
			pic_buff.clear();

		} while (false);

	}
	GlobalUnlock(hMem);
	pStream->Release();
	GlobalFree(hMem);
	image.ReleaseDC();
	return 0;
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
		INT_PTR loginRet = m_mainDlg.DoModal();
		if (loginRet == IDCANCEL) {
			// 用户点击了取消或者×
			
			break;
		}
		// m_mainDlg.DestroyWindow();
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
			root["phone"] = CTools::wcharStr2charStr(m_mainDlg.GetAccText()).c_str();
			root["password"] = CTools::wcharStr2charStr(m_mainDlg.GetPwText()).c_str();
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
			return true;
		}
	} while (true);

	return false;
}

bool CCommand::initSysTray()
{
	m_mainDlg.Create(IDD_DLG_LOGIN);
	HICON hIcon = CTools::LoadIconRes((CTools::GetAppPathW() + _T("image\\logo.ico")));
	if (hIcon == NULL) {
		// 图标资源加载失败
		TRACE(_T("图标资源加载失败 %d\n"), GetLastError());
		return false;
	}
	BOOL ret = m_sysTray.AddTray(m_mainDlg.GetSafeHwnd(), WM_TRAYICON_NOTIFY, 1, hIcon, _T("beCtrl-空闲"));
	if (ret == FALSE) {
		// 系统托盘创建失败
		TRACE(_T("%s(%d):系统托盘创建失败 %d\n"), __FILE__, __LINE__, GetLastError());
		m_mainDlg.DestroyWindow();
		CTools::DestroyIconRes(hIcon);
		return false;
	}
	CTools::DestroyIconRes(hIcon);
	return true;
}
