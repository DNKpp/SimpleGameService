#pragma once

#include "NetworkSSLServer.hpp"
#include "Database.hpp"
#include "NetworkConfig.hpp"

class Server : boost::noncopyable
{
private:
	network::SSLServer m_SSLServer;
	database::Database m_Database;

	config::Network m_NetworkConfig;

	void _readNetworkConfig();
	void _saveNetworkConfig() const;

public:
	Server();
	~Server();

	void start();
};

