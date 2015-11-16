#include "stdafx.h"
#include "Database.hpp"
#include "DatabaseConfig.hpp"
#include "DatabaseExceptions.hpp"
#include "DatabaseTables.hpp"

namespace
{
	QSqlDatabase _openDB(QString _name, QString _hostName, uint16_t _port, QString _dbName, QString _userName, QString _password)
	{
		auto db = QSqlDatabase::addDatabase("QMYSQL", _name);
		if (!db.isValid())
			throw database::ConnectionError("Unable to load SQL driver.");
		db.setHostName(_hostName);
		db.setPort(_port);
		db.setDatabaseName(_dbName);
		db.setUserName(_userName);
		db.setPassword(_password);
		if (!db.open())
			throw database::ConnectionError("Unable to connect to the SQL Database.");
		LOG_INFO("Successfully connected to database: " + _dbName + " at host: " + _hostName);
		return db;
	}
} // anonymous namespace

namespace database
{
	void Database::connect(const config::Database& _config)
	{
		if (m_UsersDatabase.isOpen())
			m_UsersDatabase.close();
		if (m_AchievementDatabase.isOpen())
			m_AchievementDatabase.close();

		m_UsersDatabase = _openDB("Users", _config.getHostName(), _config.getPort(), _config.getDatabaseName(), _config.getUserName(), _config.getPassword());
		m_AchievementDatabase = _openDB("Achievements", _config.getHostName(), _config.getPort(), _config.getDatabaseName(), _config.getUserName(), _config.getPassword());
	}
} // namespace database
