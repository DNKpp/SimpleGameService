#include "stdafx.h"
#include "NetworkTcpServer.hpp"
#include "moc_NetworkTcpServer.hpp"
#include "NetworkConfig.hpp"
#include "NetworkExceptions.h"
#include "NetworkConnection.hpp"
#include "Session.hpp"

namespace network
{
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
			auto con = new Connection(*socket, m_Server);
			auto session = new Session(con, this);
			LOG_INFO("New connection Session: " + session->getID() + " Host: " + socket->peerAddress().toString() + " Port: " + socket->peerPort());
			assert(connect(socket, SIGNAL(disconnected()), session, SLOT(_onDisconnected())));
			assert(connect(con, SIGNAL(messageReceived(const network::IMessage&)), session, SLOT(_onMessageReceived(const network::IMessage&))));
		}
	}
} // namespace network
