#pragma once

#include "Crypt.hpp"

namespace network
{
	class IMessage;
} // namespace network

class Session : public QObject
{
	Q_OBJECT
private:
	Crypt m_Crypt;
	bool m_ValidPublicKey = true;

private:
	void _handleWelcome(QByteArray _buffer);
	void _handleLoginReply(QByteArray _buffer);
	void _handleAuthenticationReply(QByteArray _buffer);
	void _handleAchievementReply(QByteArray _buffer);

public:
	bool hasValidPublicKey() const;
	void sendPacket(QByteArray _msg, network::MessageType _type);

signals:
	void send(QByteArray, network::MessageType);
	void ready();

private slots:
	void _onMessageReceived(const network::IMessage&);
};
