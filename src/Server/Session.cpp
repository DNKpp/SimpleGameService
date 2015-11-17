#include "stdafx.h"
#include "Session.h"
#include "moc_Session.h"
#include "NetworkConnection.hpp"

Session::Session(network::Connection* _con, QObject* _parent) :
	super(_parent),
	m_Connection(_con)
{
	assert(_con);
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

void Session::sendReply(QByteArray _buffer, uint32_t _type)
{
	QMutexLocker lock(&m_ConnectionMutex);
	if (m_Connection)
		m_Connection->send(_buffer, _type);
}

void Session::_onDisconnected()
{
	QMutexLocker lock(&m_ConnectionMutex);
	m_Connection = nullptr;
}
