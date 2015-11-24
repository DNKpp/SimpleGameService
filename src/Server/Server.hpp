#pragma once

#include "NetworkSSLServer.hpp"
#include "Database.hpp"

class Server : boost::noncopyable
{
private:
	network::SSLServer m_TcpServer;
	database::Database m_Database;

public:
	Server();

	void start();
};

