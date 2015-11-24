#include "stdafx.h"
#include "Server.hpp"
#include "NetworkConfig.hpp"
#include "DatabaseConfig.hpp"

const char NetConfFileName[] = "network.conf";
const char DBConfFileName[] = "database.conf";

Server::~Server()
{
	_saveDatabaseConfig();
	_saveNetworkConfig();
}

void Server::start()
{
	_readDatabaseConfig();
	m_Database.setup(m_DatabsaseConfig);

	_readNetworkConfig();
	m_SSLServer.start(m_NetworkConfig);
}

void Server::_readNetworkConfig()
{
	try
	{
		m_NetworkConfig.load(NetConfFileName);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERR("Network config error: Initializes with default values:: " + e.what());
	}
}

void Server::_saveNetworkConfig() const
{
	try
	{
		m_NetworkConfig.save(NetConfFileName);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERR("Network config error: " + e.what());
	}
}

void Server::_readDatabaseConfig()
{
	try
	{
		m_DatabsaseConfig.load(DBConfFileName);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERR("Database config error: Initializes with default values:: " + e.what());
	}
}

void Server::_saveDatabaseConfig() const
{
	try
	{
		m_DatabsaseConfig.save(DBConfFileName);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERR("Database config error: " + e.what());
	}
}
