#include "stdafx.h"
#include "Session.hpp"
#include "moc_Session.hpp"
#include "NetworkConnection.hpp"
#include "Tasks.hpp"
#include "Messages.pb.h"

uint64_t Session::SessionCounter = 0;

Session::Session(network::Connection* _con, QObject* _parent) :
	super(_parent),
	m_Connection(_con)
{
	assert(_con);
	_sendWelcomeMessage();
}

void Session::_sendWelcomeMessage()
{
	auto publicKey = m_Crypt.generateRSA();
	protobuf::Welcome msg;
	std::string keyString;
	CryptoPP::StringSink keySink(keyString);
	publicKey.Save(keySink);
	msg.set_key(keyString);
	QByteArray buffer(msg.ByteSize(), 0);
	msg.SerializeToArray(buffer.data(), buffer.size());
	sendReply(buffer, network::client::welcome);
}

uint64_t Session::getGameID() const
{
	return m_GameID;
}

void Session::setGameID(uint64_t _id)
{
	m_GameID = std::move(_id);
}

uint64_t Session::getUserID() const
{
	return m_UserID;
}

void Session::setUserID(uint64_t _id)
{
	m_UserID = std::move(_id);
}

uint64_t Session::getID() const
{
	return m_SessionID;
}

void Session::sendReply(QByteArray _buffer, network::MessageType _type)
{
	if (m_Connection)
		m_Connection->send(_buffer, _type);
}

void Session::_onDisconnected()
{
	m_Connection = nullptr;
}

void Session::_onMessageReceived(const network::IMessage& _msg)
{
	if (!_msg.isComplete() || _msg.getBytes().isEmpty())
	{
		LOG_ERR("Session: " + getID() + " Received message is empty or not complete.");
		return;
	}

	try
	{
		auto taskWatcher = std::make_unique<task::TaskWatcher>(*this, this);
		taskWatcher->run(_msg.getBytes(), _msg.getMessageType());
		taskWatcher.release();		// not needed anymore. deletes itself when finishes.
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERR(e.what());
		return;
	}
}

void Session::_onTaskFinished(QByteArray _buffer, network::MessageType _type)
{
	sendReply(_buffer, _type);
}
