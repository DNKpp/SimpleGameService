#include "stdafx.h"
#include "Session.hpp"
#include "moc_Session.hpp"
#include "Messages.pb.h"
#include "NetworkMessage.hpp"

bool Session::hasValidPublicKey() const
{
	return m_ValidPublicKey;
}

void Session::_handleWelcome(QByteArray _buffer)
{
	protobuf::Welcome msg;
	msg.ParseFromArray(_buffer.data(), _buffer.size());

	try
	{
		CryptoPP::StringSource stringSrc(msg.key(), true);
		CryptoPP::RSA::PublicKey publicKey;
		publicKey.Load(stringSrc);
		m_Crypt.setOtherRSAPublic(publicKey);
		m_ValidPublicKey = true;
		emit ready();
	}
	catch (...) {}
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

void Session::sendPacket(QByteArray _msg, network::MessageType _type)
{
	if (!hasValidPublicKey())
		std::runtime_error("No valid public key.");
	auto _encrypt = m_Crypt.encryptRSA(_msg);
	emit send(_encrypt, _type);
}

void Session::_onMessageReceived(const network::IMessage& _msg)
{
	using namespace network::client;
	auto type = _msg.getMessageType();
	if (type == welcome)
		_handleWelcome(_msg.getBytes());
	else if (!m_ValidPublicKey)
	{
		LOG_ERR("Has not received welcome message.");
		return;
	}

	switch (type)
	{
	case welcome:  break;
	case loginReply: _handleLoginReply(_msg.getBytes()); break;
	case authenticationReply: _handleAuthenticationReply(_msg.getBytes()); break;
	case achievementReply: _handleAchievementReply(_msg.getBytes()); break;
	default:
		LOG_INFO("Unhandled message received.");
	}
}
