#include "stdafx.h"
#include "NetworkSSLServer.hpp"
#include "moc_NetworkSSLServer.hpp"
#include "NetworkConfig.hpp"
#include "NetworkExceptions.h"
#include "NetworkConnection.hpp"
#include "Session.hpp"

namespace network
{
	void SSLServer::start(const config::Network& _config)
	{
		QFile file("./ssl/server.key");
		if (!file.open(QIODevice::ReadOnly))
			throw InitError(file.errorString().toStdString());
		m_SSLKey = std::make_unique<QSslKey>(&file, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, QByteArray("1234"));
		if (m_SSLKey->isNull())
			throw InitError("Unable to read ssl private key.");

		file.close();
		file.setFileName("./ssl/server.crt");
		if (!file.open(QIODevice::ReadOnly))
			throw InitError(file.errorString().toStdString());
		m_SSLCertificate = std::make_unique<QSslCertificate>(&file);
		if (m_SSLCertificate->isNull())
			throw InitError("Unable to read ssl certificate.");

		if (!listen(QHostAddress::Any, _config.getPort()))
			throw InitError(errorString().toStdString());
		LOG_INFO("TCP Server open on port: " + serverPort());
	}

	void SSLServer::incomingConnection(int _socketDescriptor)
	{
		assert(m_SSLCertificate && m_SSLKey);
		QSslSocket* socket = new QSslSocket(this);
		if (socket->setSocketDescriptor(_socketDescriptor))
		{
			// setup connection
			auto con = new Connection(*socket, this);
			connect(socket, SIGNAL(encrypted()), con, SLOT(_onSocketReady()));
			connect(socket, SIGNAL(sslErrors(const QList<QSslError>&)), con, SLOT(_onSSLErrors(const QList<QSslError>&)));

			// setup session
			auto session = new Session(this);
			LOG_INFO("New connection Session: " + session->getID() + " Host: " + socket->peerAddress().toString() + " Port: " + socket->peerPort());
			connect(socket, SIGNAL(destroyed()), session, SLOT(_onDisconnected()));
			connect(socket, SIGNAL(disconnected()), session, SLOT(_onDisconnected()));
			connect(con, SIGNAL(messageReceived(const network::IMessage&)), session, SLOT(_onMessageReceived(const network::IMessage&)));
			connect(session, SIGNAL(packetReady(const network::OMessage&)), con, SLOT(onPacketSent(const network::OMessage&)));

			// setup socket
			socket->setPrivateKey(*m_SSLKey);
			socket->setLocalCertificate(*m_SSLCertificate);
			socket->startServerEncryption();
		}
		else
			socket->deleteLater();
	}

} // namespace network
