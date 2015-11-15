#include "stdafx.h"
#include "Server.hpp"
#include "NetworkConfig.hpp"
#include "DatabaseConfig.hpp"

Server::Server() :
	m_TcpServer(m_MsgListener)
{
}

void Server::start()
{
	m_TcpServer.start(config::Network());
	m_Database.connect(config::Database());
}
