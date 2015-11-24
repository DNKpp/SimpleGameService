#pragma once

namespace network
{
	class IMessage;
	class OMessage;
} // namespace network

class Session : public QObject
{
	Q_OBJECT
private:
	void _handleLoginReply(QByteArray _buffer);
	void _handleAuthenticationReply(QByteArray _buffer);
	void _handleAchievementReply(QByteArray _buffer);

public:
	void sendPacket(QByteArray _msg, network::MessageType _type);

signals:
	void send(const network::OMessage&);

private slots:
	void _onMessageReceived(const network::IMessage&);
};
