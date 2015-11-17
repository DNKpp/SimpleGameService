#include "stdafx.h"
#include "NetworkTcpServer.hpp"
#include "moc_NetworkTcpServer.hpp"
#include "NetworkConfig.hpp"
#include "NetworkExceptions.h"
#include "MessageExecuter.hpp"
#include "NetworkConnection.hpp"
#include "Session.h"

namespace network
{
	TcpServer::TcpServer(MessageExecuter& _msgListener) :
		m_MsgListener(_msgListener)
	{
	}

	void TcpServer::start(const config::Network& _config)
	{
		m_Server = new QTcpServer(this);
		assert(connect(m_Server, SIGNAL(newConnection()), this, SLOT(_onConnect())));
		if (!m_Server->listen(QHostAddress::Any, _config.getPort()))
			throw InitError(m_Server->errorString().toStdString());
		LOG_INFO("TCP Server open on port: " + m_Server->serverPort());
	}

	void TcpServer::_onConnect()
	{
		if (!m_Server->hasPendingConnections())
			return;
		auto socket = m_Server->nextPendingConnection();
		if (socket)
		{
			LOG_INFO("New connection from host: " + socket->peerAddress().toString() + " at port: " + socket->peerPort());
			auto con = new Connection(*socket, m_Server);
			auto session = new Session(con, this);
			con->setSession(session);
			assert(connect(socket, SIGNAL(disconnected()), session, SLOT(_onDisconnected())));
			assert(connect(con, SIGNAL(messageReceived(const network::IMessage&)), &m_MsgListener, SLOT(onMessageReceived(const network::IMessage&))));
		}
	}
} // namespace network
