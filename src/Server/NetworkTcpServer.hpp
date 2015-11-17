#pragma once

namespace config
{
	class Network;
}

class MessageExecuter;

namespace network
{
	class Connection;

	class TcpServer : public QObject, boost::noncopyable
	{
		Q_OBJECT

	private:
		using super = QObject;
		QTcpServer* m_Server = nullptr;
		MessageExecuter& m_MsgListener;

	public:
		TcpServer(MessageExecuter& _msgListener);
		void start(const config::Network& _config);

	private slots:
		void _onConnect();
	};
} // namespace network
