#pragma once

#include "NetworkConnection.hpp"

namespace config
{
	class Network;
}

class MessageListener;

namespace network
{
	class TcpServer : public QObject, boost::noncopyable
	{
		Q_OBJECT

	private:
		using super = QObject;
		QTcpServer* m_Server = nullptr;
		MessageListener& m_MsgListener;

	public:
		TcpServer(MessageListener& _msgListener);
		void start(const config::Network& _config);

	private slots:
		void _onConnect();
	};
} // namespace network
