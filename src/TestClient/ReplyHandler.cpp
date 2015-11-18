#include "stdafx.h"
#include "ReplyHandler.hpp"
#include "moc_ReplyHandler.hpp"

namespace reply
{
	void Handler::_onMessageReceived(const network::IMessage&)
	{
		LOG_INFO("Reply received.");
	}
} // namespace reply
