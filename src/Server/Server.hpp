#pragma once

#include "NetworkTcpServer.hpp"
#include "MessageExecuter.hpp"

class Server : boost::noncopyable
{
private:
	MessageExecuter m_MsgListener;
	network::TcpServer m_TcpServer;

public:
	Server();

	void start();
};

