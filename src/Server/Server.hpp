#pragma once

#include "NetworkTcpServer.hpp"
#include "Database.hpp"

class Server : boost::noncopyable
{
private:
	network::TcpServer m_TcpServer;
	database::Database m_Database;

public:
	Server();

	void start();
};

