#include "WebSocket.h"

conFunc   WebSocketClient::m_pConFunc = NULL;

server* WebSocketServer::ws_server = NULL;

conFunc   WebSocketServer::m_pConFunc = NULL;
bool WebSocketClient::connection_successful = false;