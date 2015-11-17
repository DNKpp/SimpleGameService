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
		static uint64_t ConnectionID;

		QSqlDatabase m_Database;

		int m_DBIndex = 0;

	public:
		Database(const config::Database& _config);
		Database(const Database& _other);

		void open();

		QSqlDatabase getDatabase() const;
	};
} // namespace database
