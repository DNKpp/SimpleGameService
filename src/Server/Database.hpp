#pragma once

namespace config
{
	class Database;
}

namespace database
{
	class Database
	{
	private:
		QSqlDatabase m_Database;

	public:
		void setup(const config::Database& _config);
	};
} // namespace database
