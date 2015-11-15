#include "stdafx.h"
#include "MessageListener.hpp"
#include "moc_MessageListener.hpp"

MessageListener::MessageListener(QObject* _parent) :
	super(_parent)
{
}

void MessageListener::onMessageReceived(const network::IMessage& _msg)
{
}
