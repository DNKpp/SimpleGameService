#include "stdafx.h"
#include "NetworkExceptions.hpp"

namespace network
{
	Exception::Exception(const std::string& _what) :
		super(_what)
	{
	}

	InitError::InitError(const std::string& _what) :
		super(_what)
	{
	}

	MessageError::MessageError(const std::string& _what) :
		super(_what)
	{
	}

	IMessageError::IMessageError(const std::string& _what) :
		super(_what)
	{
	}

	IMessageHeaderError::IMessageHeaderError(const std::string& _what) :
		super(_what)
	{
	}

	OMessageError::OMessageError(const std::string& _what) :
		super(_what)
	{
	}
} // namespace network
