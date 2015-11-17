#include "stdafx.h"
#include "Database.hpp"
#include "DatabaseConfig.hpp"
#include "DatabaseExceptions.hpp"

namespace database
{
	uint64_t Database::ConnectionID = 0;

	Database::Database(const config::Database& _config)
	{
		++ConnectionID;

		m_Database = QSqlDatabase::addDatabase("QMYSQL", "DB");
		if (!m_Database.isValid())
			throw database::ConnectionError("Unable to load SQL driver.");
		m_Database.setHostName(_config.getHostName());
		m_Database.setPort(_config.getPort());
		m_Database.setDatabaseName(_config.getDatabaseName());
		m_Database.setUserName(_config.getUserName());
		m_Database.setPassword(_config.getPassword());
	}

	Database::Database(const Database& _other)
	{
		++ConnectionID;
		m_Database = QSqlDatabase::cloneDatabase(_other.m_Database, "DB" + QString::number(ConnectionID));
	}

	void Database::open()
	{
		if (!m_Database.open())
			throw database::ConnectionError("Unable to connect to the SQL Database.");
	}

	QSqlDatabase Database::getDatabase() const
	{
		return m_Database;
	}
} // namespace database
