#include "stdafx.h"
#include "Session.hpp"
#include "moc_Session.hpp"
#include "Messages.pb.h"
#include "NetworkMessage.hpp"

void Session::sendPacket(QByteArray _msg, network::MessageType _type)
{
	emit send(_msg, _type);
}

void Session::_handleLoginReply(QByteArray _buffer)
{
	protobuf::LoginReply msg;
	msg.ParseFromArray(_buffer.data(), _buffer.size());
	LOG_INFO("Login reply received.");
}

void Session::_handleAuthenticationReply(QByteArray _buffer)
{
	protobuf::AuthenticationReply msg;
	msg.ParseFromArray(_buffer.data(), _buffer.size());
	LOG_INFO("Authentication reply received.");
}

void Session::_handleAchievementReply(QByteArray _buffer)
{
	protobuf::AchievementReply msg;
	msg.ParseFromArray(_buffer.data(), _buffer.size());
	LOG_INFO("Achievement reply received.");
}

void Session::_onMessageReceived(const network::IMessage& _msg)
{
	using namespace network::client;
	auto type = _msg.getMessageType();

	switch (type)
	{
	case loginReply: _handleLoginReply(_msg.getBytes()); break;
	case authenticationReply: _handleAuthenticationReply(_msg.getBytes()); break;
	case achievementReply: _handleAchievementReply(_msg.getBytes()); break;
	default:
		LOG_INFO("Unhandled message received.");
	}
}
