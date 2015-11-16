#pragma once

namespace config
{
	class Network;
}

class MessageListener;

namespace network
{
	class Connection;

	class TcpServer : public QObject, boost::noncopyable
	{
		Q_OBJECT

	private:
		using super = QObject;
		QTcpServer* m_Server = nullptr;
		MessageListener& m_MsgListener;

		std::unordered_map<uint64_t/*connectionID*/, Connection*> m_Connections;

	public:
		TcpServer(MessageListener& _msgListener);
		void start(const config::Network& _config);

		Connection* findConnection(uint64_t _ID) const;

	private slots:
		void _onConnect();
		void _onDisconnected(const Connection&);
	};
} // namespace network
