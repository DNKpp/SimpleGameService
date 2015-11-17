#include "stdafx.h"
#include "Server.hpp"
#include "NetworkConfig.hpp"
#include "DatabaseConfig.hpp"

Server::Server() :
	m_Database(config::Database())
{
}

void Server::start()
{
	m_TcpServer.start(config::Network());
}
