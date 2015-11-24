#include "stdafx.h"
#include "NetworkSSLServer.hpp"
#include "moc_NetworkSSLServer.hpp"
#include "NetworkConfig.hpp"
#include "NetworkExceptions.hpp"
#include "NetworkConnection.hpp"
#include "Session.hpp"

namespace network
{
	void SSLServer::start(const config::Network& _config)
	{
		QFile file(QString::fromStdString(_config.ssl_key));
		if (!file.open(QIODevice::ReadOnly))
			throw InitError("SSL key: " + file.errorString().toStdString());
		m_SSLKey = std::make_unique<QSslKey>(&file, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, QByteArray("1234"));
		if (m_SSLKey->isNull())
			throw InitError("Unable to read ssl private key.");
		LOG_DEBUG("SSL key success.");

		file.close();
		file.setFileName(QString::fromStdString(_config.ssl_certificate));
		if (!file.open(QIODevice::ReadOnly))
			throw InitError("SSL certificate: " + file.errorString().toStdString());
		m_SSLCertificate = std::make_unique<QSslCertificate>(&file);
		if (m_SSLCertificate->isNull())
			throw InitError("Unable to read ssl certificate.");
		LOG_DEBUG("SSL certificate success.");

		if (!listen(QHostAddress::Any, _config.port))
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
