#pragma once

namespace config
{
	class Database;
}

namespace database
{
	class Database : boost::noncopyable
	{
	private:
		QSqlDatabase m_Database;

	public:
		void connect(const config::Database& _config);
	};
} // namespace database
