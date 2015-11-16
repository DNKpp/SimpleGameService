#include "stdafx.h"
#include "MessageListener.hpp"
#include "moc_MessageListener.hpp"
#include "NetworkMessage.hpp"
#include "Commands.hpp"

MessageListener::MessageListener(QObject* _parent) :
	super(_parent)
{
}

void MessageListener::onMessageReceived(const network::IMessage& _msg)
{
	if (!_msg.isComplete())
	{
		LOG_ERR("Received message is not complete.");
		return;
	}


	//commands::doCommand(static_cast<commands::Type>(_msg.getMessageType()),
	//	QByteArray::fromRawData(_msg.getBytes().data(), _msg.getBytes().size()), ConnectionInfo{_msg.getConnectionID(), _msg);
}
