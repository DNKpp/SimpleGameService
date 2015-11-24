#pragma once

#include "NetworkSSLServer.hpp"
#include "Database.hpp"
#include "NetworkConfig.hpp"
#include "DatabaseConfig.hpp"

class Server : boost::noncopyable
{
private:
	network::SSLServer m_SSLServer;
	database::Database m_Database;

	/*#####
	# config
	#####*/
	// network
	config::Network m_NetworkConfig;

	void _readNetworkConfig();
	void _saveNetworkConfig() const;

	// database
	config::Database m_DatabsaseConfig;

	void _readDatabaseConfig();
	void _saveDatabaseConfig() const;

public:
	~Server();

	void start();
};

