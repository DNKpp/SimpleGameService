#include "stdafx.h"
#include "Database.hpp"
#include "DatabaseConfig.hpp"
#include "DatabaseExceptions.hpp"

namespace database
{
	void Database::connect(const config::Database& _config)
	{
		m_Database = QSqlDatabase::addDatabase("SimpleGameService");
		m_Database.setHostName(_config.getHostName());
		m_Database.setDatabaseName(_config.getDatabaseName());
		m_Database.setUserName(_config.getUserName());
		m_Database.setPassword(_config.getPassword());
		if (!m_Database.open())
			throw ConnectionError("An unknown error occurred.");
	}
} // namespace database
