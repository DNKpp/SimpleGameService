#include "stdafx.h"
#include "DatabaseConfig.hpp"

namespace config
{
	void Database::load(const std::string& _fileName)
	{
		boost::property_tree::ptree tree;
		boost::property_tree::read_ini(_fileName, tree);

		port = tree.get("database.port", port);
		host = tree.get("database.host", host);
		database = tree.get("database.database", database);
		user = tree.get("database.user", user);
		password = tree.get("database.password", password);
	}

	void Database::save(const std::string& _fileName) const
	{
		boost::property_tree::ptree tree;

		tree.add("database.port", port);
		tree.add("database.host", host);
		tree.add("database.database", database);
		tree.add("database.user", user);
		tree.add("database.password", password);

		boost::property_tree::write_ini(_fileName, tree);
	}
} // namespace config
