#pragma once

namespace network
{
	class IMessage;
} // namespace network

namespace reply
{
	class Handler : public QObject
	{
		Q_OBJECT
	private:
		bool m_Welcome = false;

	public:
		void _handleWelcome(QByteArray _buffer);
		void _handleLoginReply(QByteArray _buffer);
		void _handleAuthenticationReply(QByteArray _buffer);
		void _handleAchievementReply(QByteArray _buffer);

	private slots:
		void _onMessageReceived(const network::IMessage&);
	};
} // namespace reply
