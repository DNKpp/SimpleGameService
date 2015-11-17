#include "stdafx.h"
#include "MessageExecuter.hpp"
#include "moc_MessageExecuter.hpp"
#include "NetworkMessage.hpp"
#include "Commands.hpp"
#include "DatabaseConfig.hpp"
#include "Tasks.h"

MessageExecuter::MessageExecuter(QObject* _parent) :
	super(_parent),
	m_Database(config::Database())
{
}

void MessageExecuter::onMessageReceived(const network::IMessage& _msg)
{
	if (!_msg.isComplete())
	{
		LOG_ERR("Received message is not complete.");
		return;
	}

	assert(_msg.getSession());

	QThreadPool::globalInstance()->start(task::createTask(_msg.getBytes(), *_msg.getSession(), m_Database, _msg.getMessageType()).release());
	//commands::doCommand(static_cast<commands::Type>(_msg.getMessageType()),
	//	QByteArray::fromRawData(_msg.getBytes().data(), _msg.getBytes().size()), ConnectionInfo{_msg.getConnectionID(), _msg);
}
