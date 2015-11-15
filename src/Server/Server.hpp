#pragma once

#include "NetworkTcpServer.hpp"
#include "MessageListener.hpp"
#include "Database.hpp"

class Server : boost::noncopyable
{
private:
	MessageListener m_MsgListener;
	network::TcpServer m_TcpServer;
	database::Database m_Database;

public:
	Server();

	void start();
};

