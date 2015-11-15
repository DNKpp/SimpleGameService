#pragma once

namespace config
{
	class Database
	{
	public:
		Database();

		QString getHostName() const;
		QString getDatabaseName() const;
		QString getUserName() const;
		QString getPassword() const;
	};
} // namespace config