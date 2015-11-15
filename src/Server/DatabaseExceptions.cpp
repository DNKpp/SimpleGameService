#include "stdafx.h"
#include "DatabaseExceptions.hpp"

namespace database
{
	Exception::Exception(const std::string& _what) :
		super(_what)
	{
	}

	ConnectionError::ConnectionError(const std::string& _what) :
		super(_what)
	{
	}
}
