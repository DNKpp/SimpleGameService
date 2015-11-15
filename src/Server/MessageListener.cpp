#include "stdafx.h"
#include "MessageListener.h"
#include "moc_MessageListener.h"

MessageListener::MessageListener(QObject* _parent) :
	super(_parent)
{
}

void MessageListener::onMessageReceived(const network::IMessage& _msg)
{
}
