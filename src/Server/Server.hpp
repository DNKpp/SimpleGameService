#pragma once

#include "NetworkTcpServer.hpp"
#include "MessageListener.h"

class Server : boost::noncopyable
{
private:
	MessageListener m_MsgListener;
	network::TcpServer m_TcpServer;

public:
	Server();

	void start();
};

