#pragma once

namespace database
{
	class Exception : public std::runtime_error
	{
		using super = std::runtime_error;
	public:
		Exception(const std::string& _what);
		virtual ~Exception() = default;
	};

	class ConnectionError : public Exception
	{
		using super = Exception;
	public:
		ConnectionError(const std::string& _what);
		virtual ~ConnectionError() = default;
	};
}
