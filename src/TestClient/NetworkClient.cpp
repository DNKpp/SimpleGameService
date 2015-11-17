#include "stdafx.h"
#include "NetworkClient.h"
#include "moc_NetworkClient.h"
#include "NetworkMessage.hpp"
#include <QtCore/QDataStream>
#include "Messages.pb.h"
#include "NetworkConnection.hpp"
#include "MessageTypes.hpp"

namespace
{
	QByteArray _createLogin()
	{
		protobuf::Login msg;
		msg.set_name("anti-freak");
		QByteArray password;
		password.append(1);
		password = QCryptographicHash::hash(password, QCryptographicHash::Sha256);
		msg.set_password(password.data(), password.size());
		QByteArray buffer(msg.ByteSize(), 0);
		msg.SerializeToArray(buffer.data(), buffer.size());
		return buffer;
	}

	QByteArray _createMessage()
	{
		protobuf::Authentication msg;
		msg.set_id(1);
		QByteArray hash;
		hash.append(52);
		hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
		msg.set_hash(hash.data(), hash.size());
		QByteArray buffer(msg.ByteSize(), 0);
		msg.SerializeToArray(buffer.data(), buffer.size());
		return buffer;
	}
} // anonymous namespace

NetworkClient::NetworkClient()
{
	m_Socket = new QTcpSocket(this);
	m_Socket->connectToHost(QHostAddress::LocalHost, 2015);
	connect(m_Socket, SIGNAL(connected()), this, SLOT(_onConnected()));

	//connect(con, SIGNAL(messageComplete(const network::IMessage&)), &m_MsgListener, SLOT(onMessageReceived(const network::IMessage&)));
}

void NetworkClient::_onConnected()
{
	qDebug() << "connected to address: " << m_Socket->peerAddress() << " port: " << m_Socket->peerPort();
	m_Connection = new network::Connection(*m_Socket, this);
	m_Connection->send(_createMessage(), network::svr::authentication);
	m_Connection->send(_createLogin(), network::svr::login);
}
