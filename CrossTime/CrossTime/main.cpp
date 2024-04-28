#include <cstdio>
#include "CrossTimeServer.h"
#include "Packet.h"

int main()
{
	if (CTools::SwitchDeamon() >= 0) {
		CCrossTimeServer server;
		server.Start();
	}
	else {
		LOG_ERROR << "守护进程创建失败";
		CCrossTimeServer::m_logFile->flush();
	}
	return 0;
}