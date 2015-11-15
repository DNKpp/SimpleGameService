#include "stdafx.h"
#include "NetworkTcpServer.hpp"
#include "moc_NetworkTcpServer.hpp"
#include "NetworkConfig.hpp"
#include "NetworkExceptions.h"
#include "MessageListener.h"

namespace network
{
	TcpServer::TcpServer(MessageListener& _msgListener) :
		m_MsgListener(_msgListener)
	{
	}

	void TcpServer::_onConnect()
	{
		if (!m_Server->hasPendingConnections())
			return;
		auto socket = m_Server->nextPendingConnection();
		if (socket)
		{
			auto con = new Connection(*socket, m_Server);
			connect(con, SIGNAL(messageComplete(const network::IMessage&)), &m_MsgListener, SLOT(onMessageReceived(const network::IMessage&)));
		}
	}

	void TcpServer::start(const config::Network& _config)
	{
		m_Server = new QTcpServer(this);
		assert(connect(m_Server, SIGNAL(newConnection()), this, SLOT(_onConnect())));
		if (!m_Server->listen(QHostAddress::Any, _config.getPort()))
			throw InitError(m_Server->errorString().toStdString());
		LOG_INFO("TCP Server open on port: " + m_Server->serverPort());
	}
} // namespace network
