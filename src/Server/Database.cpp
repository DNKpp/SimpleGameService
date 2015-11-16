#include "stdafx.h"
#include "Database.hpp"
#include "DatabaseConfig.hpp"
#include "DatabaseExceptions.hpp"

namespace database
{
	void Database::connect(const config::Database& _config)
	{
		if (m_Database.isOpen())
			m_Database.close();
		m_Database = QSqlDatabase::addDatabase("QMYSQL");
		if (!m_Database.isValid())
			throw ConnectionError("Unable to load SQL driver.");
		m_Database.setHostName(_config.getHostName());
		m_Database.setPort(_config.getPort());
		m_Database.setDatabaseName(_config.getDatabaseName());
		m_Database.setUserName(_config.getUserName());
		m_Database.setPassword(_config.getPassword());
		if (!m_Database.open())
			throw ConnectionError("Unable to connect to the SQL Database.");
		LOG_INFO("Successfully connected to database: " + _config.getDatabaseName() + " at host: " + _config.getHostName());
	}
} // namespace database
