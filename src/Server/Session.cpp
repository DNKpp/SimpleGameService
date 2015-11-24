#include "stdafx.h"
#include "Session.hpp"
#include "moc_Session.hpp"
#include "NetworkConnection.hpp"
#include "Tasks.hpp"
#include "Messages.pb.h"

uint64_t Session::SessionCounter = 0;

Session::Session(QObject* _parent) :
	super(_parent)
{
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

void Session::sendPacket(QByteArray _buffer, network::MessageType _type)
{
	network::OMessage msg;
	msg.setup(1, _type, _buffer);
	emit packetReady(msg);
}

void Session::_deleteSelfIfNecessary()
{
	// delete self if no tasks working and not connected
	if (m_TaskCounter == 0 && m_Disconnected)
		deleteLater();
}

void Session::_onDisconnected()
{
	m_Disconnected = true;
	_deleteSelfIfNecessary();
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
		++m_TaskCounter;
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERR(e.what());
		return;
	}
}

void Session::_onTaskFinished(QByteArray _buffer, network::MessageType _type)
{
	sendPacket(_buffer, _type);
	--m_TaskCounter;
	_deleteSelfIfNecessary();
}
