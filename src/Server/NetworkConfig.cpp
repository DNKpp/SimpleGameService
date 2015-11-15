#include "stdafx.h"
#include "NetworkConfig.hpp"

namespace config
{
	Network::Network()
	{
	}

	uint16_t Network::getPort() const
	{
		return 2015;
	}
} // namespace config
