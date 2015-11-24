#pragma once

namespace config
{
	class Database
	{
	public:
		std::string host = "localhost";
		uint16_t port = 3306;
		std::string database = "SimpleGameService";
		std::string user = "root";
		std::string password = "";

		void load(const std::string& _fileName);
		void save(const std::string& _fileName) const;
	};
} // namespace config