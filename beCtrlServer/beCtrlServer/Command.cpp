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
	// ���������ļ�
	bool ret = m_userCfgInfo.LoadFile(CONFIG_FILE_NAME);
	if (ret == false) {
		// ��������ļ���������߲�����
		// ����������ļ����¼��ػ�����������
		USER_CONFIG_INFO uci;
		m_userCfgInfo.AddCfg(uci);
		if (false == m_userCfgInfo.SaveFile(CONFIG_FILE_NAME)) {
			TRACE("file create fail\r\n");
			MessageBoxA(NULL, "�ļ�����ʧ�ܣ����Թ���ԱȨ�����У�", "���ô���", MB_OK | MB_USERICON);
			exit(0);
		}
	}
	else {
		m_pUserCfgInfo = m_userCfgInfo.GetUserCfgInfo();
		// ���õ�¼��Ϣ
		m_loginDlg.SetAccText(m_pUserCfgInfo->szUser);
		m_loginDlg.SetPwText(m_pUserCfgInfo->szPwd);
		m_loginDlg.SetRememberPwd(m_pUserCfgInfo->bRememberPwd);
		m_loginDlg.SetAutoLogin(m_pUserCfgInfo->bAutoLogin);
	}
	// �����ʼ�����з���������
	if (false == initCmd()) {
		Close();
		exit(0);
	}
	m_wsClient.connect();  // ���ӷ�����
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
	// ��utf-8�����ת�����
	std::string data = CTools::from_utf8(msg->get_payload());
	// std::string data = msg->get_payload();
	// std::cout << "(548)Received message: " << data.c_str() << std::endl;

	TRACE("Received message:[%s]\r\n", data.c_str());

	// data��json����
	// ����json���ݵĽ���
	Json::Value root;
	Json::Reader reader;
	if (false == reader.parse(data, root)) {
		// ����ʧ��
		printf("<%s>(%d):[error]json ���ݽ���ʧ�� data:[%s]\r\n", __FILE__, __LINE__, data.c_str());
		return;
	}
	std::string fields[] = { "opType","dir","auth" };

	if ((false == root.isMember(fields[0]))
		|| (false == root.isMember(fields[1]))
		|| (false == root.isMember(fields[2]))
		) {
		TRACE(_T("�������Ͳ�ƥ��\r\n"));
		return;
	}
	int type = atoi(root[fields[0]].asString().c_str());
	std::string dir = root[fields[1]].asString();
	std::string toCtrl = root[fields[2]].asString();
	Json::Value retData;
	switch (type)
	{
	case 1:  //  �ļ�Ԥ��
		retData = GetFiles(dir, &m_wsClient);
		retData["toCtrl"] = toCtrl;
		retData["opType"] = root[fields[0]].asString();
		retData["equip"] = "sb1";
		m_wsClient.send(retData.toStyledString(), websocketpp::frame::opcode::text);
		break;
	case 2:  //  �ļ�ɾ��
		break;
	case 3: // �ļ��½�
		break;
	case 4: // �ļ�ִ��
		break;
	case 5: // �ļ�������
		break;
	case 6: // �ļ��ϴ�
		FileUpload(root, &m_wsClient);
		break;
	case 7:  // �ļ�����
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

	// ����������˭
	// ws_client->send(hdl, data.c_str(), websocketpp::frame::opcode::text);

	m_wsClient.send(retData.toStyledString(), websocketpp::frame::opcode::text);

	TRACE(_T("ws���������ӳɹ�\n"));
}

Json::Value CCommand::GetFiles(const std::string& dir, WebSocketClient* clien)
{

	Json::Value res; // Ҫ���صĽ��
	Json::Value arrFileInfo; // �ļ���Ϣ��������
	Json::Value fItem;   // ÿһ���ļ���Ϣ����
	BOOL ret;   // ����ֵ
	if (dir == "") {
		// �鿴��
		DWORD len = 0;
		char VolumeName[MAX_PATH] = "";
		HANDLE FindDriHandle = FindFirstVolumeA(VolumeName, sizeof(VolumeName)); // ��ȡ��һ������������Ϣ
		if (FindDriHandle == INVALID_HANDLE_VALUE) {
			TRACE("���̻�ȡʧ�ܣ�\r\n");
			goto BREAK;
		}

		do {
			// �жϸ�ʽ�Ƿ���ȷ
			size_t index = strlen(VolumeName) - 1;
			if (!VolumeFormat(VolumeName, "\\\\?\\") || VolumeName[index] != '\\') {
				TRACE(_T("���̻�ȡʧ��\r\n"));
				break;
			}
			char driveName[MAX_PATH] = "";
			GetVolumePathNamesForVolumeNameA(VolumeName, driveName, sizeof(driveName), &len); // ��ȡ������
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
		// �鿴ָ��Ŀ¼�µ��ļ�
		std::string findDir = dir + '*'; // C://*
		TRACE("dir:[%s]\r\n", findDir);
		WIN32_FIND_DATAA fileInfo = {};
		HANDLE findFileHandle = FindFirstFileA(findDir.c_str(), &fileInfo);
		if (findFileHandle == INVALID_HANDLE_VALUE) {
			// ����ʧ��
			TRACE("�鿴ָ��Ŀ¼�µ��ļ�ʧ�� dir:[%s]\r\n", findDir.c_str());
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
	"auth"   : "...",           // �ֻ���
	"opType" : "0/1/2/3/4/5/6/7", // �������� ����/�ļ����/ɾ��/�½�/����/������/�ϴ�/����
	"EName"  : "...",           // �豸��
	"bNext"  : "0/1",           // �Ƿ��к���  0��ʾû��/1��ʾ��
	"dir"    : "...",           // �ļ�����·��
	"data"   : "..."            // �ļ����ݶ����Ƹ�ʽ
}*/
	Json::Value res; // Ҫ���صĽ��

	return res;
}

Json::Value CCommand::FileDownload(const Json::Value& data, WebSocketClient* clien)
{
	/*{
	"auth"   : "...",           // �ֻ���
	"opType" : "0/1/2/3/4/5/6/7", // �������� ����/�ļ����/ɾ��/�½�/����/������/�ϴ�/����
	"EName"  : "...",           // �豸��
	"dir"    : "..."            // �ļ�����·��
}*/
	TRACE("���뵽����\r\n");
	std::string fields[] = { "dir","auth","opType" };
	Json::Value res; // Ҫ���صĽ��
	do {
		if (false == data.isMember(fields[0])) {
			// �ļ����ز�����·���ֶ�
			TRACE("�ļ����ز�����·���ֶ�\r\n");
			break;
		}
		if (false == data[fields[0]].isString()) {
			TRACE("�������Ͳ�ƥ��\r\n");
			break;
		}
		std::string dir = data[fields[0]].asString();
		// ���ļ�
		FILE* pFile = fopen(dir.c_str(), "rb");
		// HANDLE hFile = CreateFileA(dir.c_str(), GENERIC_READ, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (pFile == NULL) {
			TRACE("�ļ���ʧ��\r\n");
			break;
		}
		// ��ȡ�ļ���С
		fseek(pFile, 0, SEEK_END);
		long long FileSize = _ftelli64(pFile);
		fseek(pFile, 0, SEEK_SET);
		if (FileSize == -1) {
			TRACE("�ļ���С��ȡʧ��!\r\n");
			break;
		}
		TRACE("�ļ��ܴ�С %d\r\n", FileSize);

		// ���ļ���Ƭ�������ƶ�
		long long totalSize = FileSize;
		char buffer[1024] = "";

		std::string toCtrl = data[fields[1]].asString();
		char ToCtrl[20] = "";

		memcpy(ToCtrl, toCtrl.c_str(), (toCtrl.size() > 19) ? 19 : toCtrl.size());

		size_t RLen = 0;
		do {
			memset(buffer, 0, sizeof(buffer));
			RLen = fread(buffer, 1, sizeof(buffer), pFile);

			TRACE("������%d���ֽ�\r\n", RLen);

			/*if (FALSE == ReadFile(hFile, buffer, sizeof(buffer), &RLen, NULL)) {
				TRACE("�ļ���ȡʧ��\n");
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
			TRACE("һ�����ֽ��� %d\r\n", Buf.size());
			//if (FileSize > RLen) {
			//	// ���к���
			//	res["bNext"] = 1;
			//	res["data"] = Buf;
			//}
			//else {
			//	// ���һ����
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
		TRACE(_T("�̴߳���ʧ��\n"));
		return -1;
	}

	// ����webSocket�ͻ����߳�
	m_hWsClient = (HANDLE)_beginthread(&CCommand::wsClientThread, 0, this);
	if (m_hWsClient == INVALID_HANDLE_VALUE) {
		// ����Ӧ�ð����洴���ɹ����̹߳ر�
		Stop();
		TRACE(_T("�̴߳���ʧ�ܣ�\n"));
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
			TRACE(_T("�߳�û�Զ��رգ��ѱ�ǿ�ƹر�\n"));
			TerminateThread(hCommand, 1);
		}
	}
	// �ر�ws�ͻ����߳�
	if (m_hWsClient != INVALID_HANDLE_VALUE) {
		HANDLE hWsClient = m_hWsClient;
		m_hWsClient = INVALID_HANDLE_VALUE;
		m_wsClient.Stop();
		if (WAIT_TIMEOUT == WaitForSingleObject(hWsClient, 500)) {
			// �߳̽����ȴ���ʱ����Ҫǿ�ƽ��߳̽�����
			TRACE(_T("�߳�û�Զ��رգ��ѱ�ǿ�ƹر�\n"));
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
			TRACE("�����ɹ� cmd:%d\r\n", pack.GetCmd());
			switch (pack.GetCmd())
			{
			case CTC_STARTEQUIPCTRL: {// �����Ļ
				// �������ݰ������m_num���Ƕ˿ں�
				m_imageSendPort = pack.GetNum();
				m_imageSendName = pack.GetData();
				TRACE(_T("��ȡ�����Ͷ˿ڣ�%d  %s\n"), m_imageSendPort, m_imageSendName.c_str());
				StartImageSend();
				break;
			}
			case CTC_ENDEQUIPCTRL: {// ֹͣ���
				// printf("%s(%d):��ֹͣ��Ļ��Ϣ����\n", __FILE__, __LINE__);
				// TRACE(_T("mouse total %lld\n"), mtotal);
				StopImageSend();
				break;
			}
			case CTC_CTRLEVENT: { // ��������Ϣ
				// data����������������
				MKEvent ME;
				memcpy(&ME, pack.GetData(), sizeof(ME));
				MKEventDispose(ME);
				// printf("�������¼��������:[x:%d, y:%d]\n", ME.MouseXY.x, ME.MouseXY.y);
				// mtotal++;
				break;
			}
			case CTC_EQUIPSHUTDOWN: { // �豸�ػ�����
				TRACE("Ҫ��ػ���ʱ�� %s\r\n", pack.GetData());   // TODO: ������ת��ΪС������
				const char* message = "���������1���Ӻ�رգ��뱣�����Ĺ������˳�����\n\
����Ҽ����̳����ػ�����\n\
			������������CTCtrl�����Ŷ�";
				CTools::LocalSystemShutdown(message);

				Sleep(5000);

				// ȡ���¼�
				CTools::RevocationLocalSystemShutdown();
				TRACE("�����ѳ��� %s\r\n", pack.GetData());
				break;
			}
			case CTC_EQUIPRESTART: {  // �豸��������
				TRACE("Ҫ��������ʱ�� %s\r\n", pack.GetData());
				const char* message = "���������1���Ӻ��������뱣�����Ĺ������˳�����\n\
����Ҽ����̳�����������\n\
			������������CTCtrl�����Ŷ�";
				CTools::LocalSystemShutdown(message, true);

				Sleep(5000);

				// ȡ���¼�
				CTools::RevocationLocalSystemShutdown();
				TRACE("�����ѳ��� %s\r\n", pack.GetData());
				break;
			}
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
		// TRACE(_T("��ǰͼƬ��СΪ %lld\n"), ImageData.m_data.size());

		TRACE(_T("��ǰͼƬ��СΪ %lld\n"), imageData.size());

		// ͼƬ��
		// ͼƬ����
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
	TRACE(_T("%s(%d):ֹͣ��Ļ��Ϣ����\n"), __FILE__, __LINE__);
	closesocket(m_sockSendImage);
	m_sockSendImage = INVALID_SOCKET;
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
		INT_PTR loginRet = m_loginDlg.DoModal();
		TRACE("%d, %d\n", m_loginDlg.GetRememberPwd(), m_loginDlg.GetAutoLogin());
		if (loginRet == IDCANCEL) {
			// �û������ȡ�����ߡ�

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
			// �ѵ�¼���ñ��浽�ļ���
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
		// ͼ����Դ����ʧ��
		TRACE(_T("ͼ����Դ����ʧ�� %d\n"), GetLastError());
		return false;
	}
	BOOL ret = m_sysTray.AddTray(m_loginDlg.GetSafeHwnd(), WM_TRAYICON_NOTIFY, 1, hIcon, _T("beCtrl-����"));
	if (ret == FALSE) {
		// ϵͳ���̴���ʧ��
		TRACE(_T("%s(%d):ϵͳ���̴���ʧ�� %d\n"), __FILE__, __LINE__, GetLastError());
		m_loginDlg.DestroyWindow();
		CTools::DestroyIconRes(hIcon);
		return false;
	}
	CTools::DestroyIconRes(hIcon);
	return true;
}
