#include "stdafx.h"
#include "DatabaseConfig.hpp"

namespace config
{
	Database::Database()
	{
	}

	QString Database::getHostName() const
	{
		return "localhost";
	}

	QString Database::getDatabaseName() const
	{
		return "SimpleGameService";
	}

	QString Database::getUserName() const
	{
		return "root";
	}

	QString Database::getPassword() const
	{
		return "tbc";
	}

	uint16_t Database::getPort() const
	{
		return 3306;
	}
}
