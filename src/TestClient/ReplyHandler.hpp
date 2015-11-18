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
	public:


	private slots:
		void _onMessageReceived(const network::IMessage&);
	};
} // namespace reply
