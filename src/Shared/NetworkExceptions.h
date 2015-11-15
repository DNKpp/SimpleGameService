#pragma once

#include <stdexcept>

namespace network
{
	class Exception : public std::runtime_error
	{
		using super = std::runtime_error;
	public:
		Exception(const std::string& _what);
		virtual ~Exception() = default;
	};

	class InitError : public Exception
	{
		using super = Exception;
	public:
		InitError(const std::string& _what);
		virtual ~InitError() = default;
	};

	class MessageError : public Exception
	{
		using super = Exception;
	public:
		MessageError(const std::string& _what);
		virtual ~MessageError() = default;
	};

	class IMessageError : public MessageError
	{
		using super = MessageError;
	public:
		IMessageError(const std::string& _what);
		virtual ~IMessageError() = default;
	};
	class IMessageHeaderError : public IMessageError
	{
		using super = IMessageError;
	public:
		IMessageHeaderError(const std::string& _what);
		virtual ~IMessageHeaderError() = default;
	};
} // namespace network
