#pragma once

namespace config
{
	class Network
	{
	public:
		uint16_t port = 2000;
		std::string ssl_key = "./ssl/server.key";
		std::string ssl_certificate = "./ssl/server.crt";

	public:
		void load(const std::string& _fileName);
		void save(const std::string& _fileName) const;
	};
} // namespace config
