#include "stdafx.h"
#include "NetworkConfig.hpp"

namespace config
{
	void Network::load(const std::string& _fileName)
	{
		boost::property_tree::ptree tree;
		boost::property_tree::read_ini(_fileName, tree);

		port = tree.get("network.port", port);

		ssl_certificate = tree.get("ssl.certificate", ssl_certificate);
		ssl_key = tree.get("ssl.key", ssl_key);
	}

	void Network::save(const std::string& _fileName) const
	{
		boost::property_tree::ptree tree;

		tree.add("network.port", port);

		tree.add("ssl.certificate", ssl_certificate);
		tree.add("ssl.key", ssl_key);

		boost::property_tree::write_ini(_fileName, tree);
	}
} // namespace config
