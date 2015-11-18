#include "stdafx.h"
#include "NetworkClient.hpp"
#include "moc_NetworkClient.hpp"
#include "Messages.pb.h"
#include "NetworkConnection.hpp"

namespace
{
	QByteArray _createLogin(const std::string& _name)
	{
		protobuf::Login msg;
		msg.set_name(_name);
		QByteArray password;
		password.append(1);
		password = QCryptographicHash::hash(password, QCryptographicHash::Sha256);
		msg.set_password(password.data(), password.size());
		QByteArray buffer(msg.ByteSize(), 0);
		msg.SerializeToArray(buffer.data(), buffer.size());
		LOG_INFO("Try login.");
		return buffer;
	}

	QByteArray _createAuthenticate()
	{
		protobuf::Authentication msg;
		msg.set_id(1);
		QByteArray hash;
		hash.append(0x52);
		hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
		msg.set_hash(hash.data(), hash.size());
		QByteArray buffer(msg.ByteSize(), 0);
		msg.SerializeToArray(buffer.data(), buffer.size());
		LOG_INFO("Try authenticate.");
		return buffer;
	}

	QByteArray _createAchievement(uint64_t id)
	{
		protobuf::Achievement msg;
		msg.set_id(id);
		msg.set_type(protobuf::Achievement_Type_add);
		QByteArray buffer(msg.ByteSize(), 0);
		msg.SerializeToArray(buffer.data(), buffer.size());
		LOG_INFO("Try achievement.");
		return buffer;
	}
} // anonymous namespace

namespace network
{
	Client::Client()
	{
		m_Socket = new QTcpSocket(this);
		m_Socket->connectToHost(QHostAddress::LocalHost, 2015);
		connect(m_Socket, SIGNAL(connected()), this, SLOT(_onConnected()));
	}

	void Client::_onConnected()
	{
		qDebug() << "connected to address: " << m_Socket->peerAddress() << " port: " << m_Socket->peerPort();
		m_Connection = new network::Connection(*m_Socket, this);
		assert(connect(m_Connection, SIGNAL(messageReceived(const network::IMessage&)), this, SLOT(_onMessageReceived(const network::IMessage&))));
		m_Connection->send(_createAuthenticate(), network::svr::authentication);
		m_Connection->send(_createLogin("test"), network::svr::login);
		m_Connection->send(_createAchievement(1), network::svr::achievement);
	}

	void Client::_onMessageReceived(const network::IMessage& _msg)
	{
		emit messageReceived(_msg);
	}
} // namespace network
