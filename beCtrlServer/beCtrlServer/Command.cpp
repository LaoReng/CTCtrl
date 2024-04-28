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
		// ������û��ֵ
		return -1;
	}
	if (mainId != ERROR_INVALID_THREAD_ID) {
		// mainId��ֵ
		m_mainThreadId = mainId;
	}
	// ����֮ǰ�Ȱ��׽��ֱ�ɷ������ģ���ǰ״̬һ���������Ϸ������˲��ɹ���¼
	int ret = m_sock.setUnlock(); // ����Ϊ�������׽���
	if (ret != 0)return -1;
	// ����Ҫ���һ���߳�
	m_hCommand = (HANDLE)_beginthread(&CCommand::commandThreadEnt, 0, this);
	if (m_hCommand == INVALID_HANDLE_VALUE) {
		TRACE(_T("%s(%d):�̴߳���ʧ��\n"), __FILE__, __LINE__);
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
		// ���������ӳɹ�

		return 0;
	}
	return -1;
}

int CCommand::Stop()
{
	// �����Ļ��ؿ��������ȹرո��߳�
	StopImageSend();

	if ((m_hCommand != INVALID_HANDLE_VALUE) && (m_bCommandRun == true)) {
		m_bCommandRun = false;
		HANDLE hCommand = m_hCommand;
		m_hCommand = INVALID_HANDLE_VALUE;
		if (WAIT_TIMEOUT == WaitForSingleObject(hCommand, INFINITE)) {
			// �߳̽����ȴ���ʱ����Ҫǿ�ƽ��߳̽�����
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
	// �������ӷ�����
	int ret = Connect();
	if (ret < 0) {
		MessageBox(NULL, _T("��ǰ�����������ߣ����Ժ����\n���������Ƿ�����"), _T("����������"), MB_OK | MB_USERICON);
		return false;
	}
	// Ȼ�����û����е�¼
	// ����һ��http���ݱ�ʾ���Ǳ��ض�
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
					// ֤�����滹������
					len = 0;
				}
				else {
					Sleep(50);
					continue;
				}
			}
			else {
				TRACE(_T("���õ�����\n"));
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
			// �����ɹ�
			memmove(pData, pData + len, size_t(BUFFER_SIZE - len));
			switch (pack.GetCmd())
			{
			case 1: // �����Ļ
			// �������ݰ������m_num���Ƕ˿ں�
				m_imageSendPort = pack.GetNum();
				// printf("��ȡ�����Ͷ˿ڣ�%d\n", m_imageSendPort);
				StartImageSend();
				break;
			case 2: // ֹͣ���
				// printf("%s(%d):��ֹͣ��Ļ��Ϣ����\n", __FILE__, __LINE__);
				// TRACE(_T("mouse total %lld\n"), mtotal);
				StopImageSend();
				break;
			case 3: // ��������Ϣ
				// data����������������
				MKEvent ME;
				memcpy(&ME, pack.GetData(), sizeof(ME));
				MKEventDispose(ME);
				// printf("�������¼��������:[x:%d, y:%d]\n", ME.MouseXY.x, ME.MouseXY.y);
				// mtotal++;
				break;
			}
		}
		else {
			// ���ݰ�У���ʧ��
			len = -1 * len;
			memmove(pData, pData + len, size_t(BUFFER_SIZE - len));
		}
		index -= len;
	}
	// TRACE(_T("�߳��˳� %d %ld\n"), WSAGetLastError(), WSAEWOULDBLOCK);
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
		TRACE(_T("�׽��ִ���ʧ��\n"));
		return;
	}
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(m_serverIp.c_str());
	addr.sin_port = htons(m_imageSendPort);
	if (connect(m_sockSendImage, (sockaddr*)&addr, sizeof(addr)) != 0) {
		TRACE(_T("����ʧ��\n"));
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
		TRACE(_T("��ǰͼƬ��СΪ %lld\n"), imageData.size());
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
	TRACE(_T("%s(%d):ֹͣ��Ļ��Ϣ����\n"), __FILE__, __LINE__);
	closesocket(m_sockSendImage);
	m_sockSendImage = INVALID_SOCKET;
}

int CCommand::SaveScreen(std::string& imageData, cv::Rect& recv)
{
	// ����CImage��ʵ����Ļ��ͼ
	HDC hDC = GetDC(NULL);
	// �ڴ���ʱ������ָ���ߴ��Image
	CImage image;
	if (image.Create(recv.width, recv.height, GetDeviceCaps(hDC, BITSPIXEL)) == FALSE) {
		TRACE(_T("ͼƬ����ʧ��\r\n"));
		return -1;
	}
	// ����ָ���������Ļ����Image��
	if (BitBlt(image.GetDC(), 0, 0, image.GetWidth(), image.GetHeight(), hDC, 0, 0, SRCCOPY) == FALSE) {
		TRACE(_T("ͼƬ����ʧ��\r\n"));
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
		TRACE(_T("�ڴ�󶨴���ʧ��\r\n"));
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
		// ���������¼�
		// ���ƶ���굽ָ��λ��
		if ((mke.MouseXY.x < 0) || (mke.MouseXY.y < 0)) {
			return -1;
		}
		// �������λ��
		SetCursorPos(mke.MouseXY.x, mke.MouseXY.y);
		input.type = INPUT_MOUSE;
		switch (mke.MKOperation)
		{
		case 0X11: // �Ҽ�����
			input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			// printf("%s(%d):����Ҽ�����\n", __FILE__, __LINE__);
			break;
		case 0X12: // �м�����
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			// printf("%s(%d):����м�����\n", __FILE__, __LINE__);
			break;
		case 0X14: // �������
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			// printf("%s(%d):����������\n", __FILE__, __LINE__);
			break;
		case 0X21: // �Ҽ�̧��
			input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			// printf("%s(%d):����Ҽ�̧��\n", __FILE__, __LINE__);
			break;
		case 0X22: // �м�̧��
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			// printf("%s(%d):����м�̧��\n", __FILE__, __LINE__);
			break;
		case 0X24: // ���̧��
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			// printf("%s(%d):������̧��\n", __FILE__, __LINE__);
			break;
		case 0X40: // ���ֹ���
			input.mi.dwFlags = MOUSEEVENTF_WHEEL;
			input.mi.mouseData = mke.MKKeyValue * (-1);
			TRACE(_T("%s(%d):������̧�� %ld\n"), __FILE__, __LINE__, input.mi.mouseData);
			break;
		default:
			goto BREAK;
			break;
		}
		input.mi.dwExtraInfo = GetMessageExtraInfo();
	}
	else if (mke.EType == 2) {
		input.type = INPUT_KEYBOARD;
		// ���²�������ki.dwFlags
		if (mke.MKOperation & 0X20) { // ̧���¼�
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			TRACE(_T("%s(%d):����̧��\n"), __FILE__, __LINE__);
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
		// �����ٴο���
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
				// �߳̽����ȴ���ʱ����Ҫǿ�ƽ��߳̽�����
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
		// �Ƚ��е�¼����
		INT_PTR loginRet = m_mainDlg.DoModal();
		if (loginRet == IDCANCEL) {
			// �û������ȡ�����ߡ�
			
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

		// �����ݽ��н������ж��Ƿ��¼�ɹ�
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
			TRACE(_T("���ݽ���ʧ��\n"));
			break;
		}
		if (false == rootret.isMember("status")) {
			// ���ݲ�ƥ��
			break;
		}
		if (true == rootret["status"].isArray()) {
			// ����Ϊ����ֵ
			break;
		}
		int status = rootret["status"].asInt();
		if (status != 0) {
			// ��¼ʧ��
			std::wstring firstLine = L"֪ͨ";
			std::wstring secondLine = L"��¼ʧ��,�����µ�¼\n���������룬�뵽��ҳ���һ�";
			CToast::ShowNotification(firstLine, secondLine);
			continue;
		}
		else {
			std::wstring firstLine = L"֪ͨ";
			std::wstring secondLine = L"��¼�ɹ�";
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
		// ͼ����Դ����ʧ��
		TRACE(_T("ͼ����Դ����ʧ�� %d\n"), GetLastError());
		return false;
	}
	BOOL ret = m_sysTray.AddTray(m_mainDlg.GetSafeHwnd(), WM_TRAYICON_NOTIFY, 1, hIcon, _T("beCtrl-����"));
	if (ret == FALSE) {
		// ϵͳ���̴���ʧ��
		TRACE(_T("%s(%d):ϵͳ���̴���ʧ�� %d\n"), __FILE__, __LINE__, GetLastError());
		m_mainDlg.DestroyWindow();
		CTools::DestroyIconRes(hIcon);
		return false;
	}
	CTools::DestroyIconRes(hIcon);
	return true;
}
