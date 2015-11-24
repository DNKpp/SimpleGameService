#include "stdafx.h"
#include "Database.hpp"
#include "DatabaseConfig.hpp"
#include "DatabaseExceptions.hpp"

namespace database
{
	void Database::setup(const config::Database& _config)
	{
		m_Database = QSqlDatabase::addDatabase("QMYSQL", "DB");
		if (!m_Database.isValid())
			throw database::ConnectionError("Unable to load SQL driver.");
		m_Database.setHostName(QString::fromStdString(_config.host));
		m_Database.setPort(_config.port);
		m_Database.setDatabaseName(QString::fromStdString(_config.database));
		m_Database.setUserName(QString::fromStdString(_config.user));
		m_Database.setPassword(QString::fromStdString(_config.password));
		LOG_DEBUG("Database setup success.");
	}
} // namespace database
