#include "stdafx.h"
#include "ReplyHandler.hpp"
#include "moc_ReplyHandler.hpp"
#include "Messages.pb.h"
#include "NetworkMessage.hpp"

namespace reply
{
	void Handler::_handleWelcome(QByteArray _buffer)
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
		}
		catch (...) {}
	}

	void Handler::_handleLoginReply(QByteArray _buffer)
	{
		protobuf::LoginReply msg;
		msg.ParseFromArray(_buffer.data(), _buffer.size());
		LOG_INFO("Login reply received.");
	}

	void Handler::_handleAuthenticationReply(QByteArray _buffer)
	{
		protobuf::AuthenticationReply msg;
		msg.ParseFromArray(_buffer.data(), _buffer.size());
		LOG_INFO("Authentication reply received.");
	}

	void Handler::_handleAchievementReply(QByteArray _buffer)
	{
		protobuf::AchievementReply msg;
		msg.ParseFromArray(_buffer.data(), _buffer.size());
		LOG_INFO("Achievement reply received.");
	}

	void Handler::_onMessageReceived(const network::IMessage& _msg)
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
} // namespace reply
