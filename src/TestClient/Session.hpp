#pragma once

namespace network
{
	class IMessage;
} // namespace network

class Session : public QObject
{
	Q_OBJECT
private:
	void _handleLoginReply(QByteArray _buffer);
	void _handleAuthenticationReply(QByteArray _buffer);
	void _handleAchievementReply(QByteArray _buffer);

	bool m_AESReply = false;

public:
	void sendPacket(QByteArray _msg, network::MessageType _type);

signals:
	void send(QByteArray, network::MessageType);

private slots:
	void _onMessageReceived(const network::IMessage&);
};
