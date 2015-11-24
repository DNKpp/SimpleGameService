#pragma once

namespace config
{
	class Network;
}

class MessageExecuter;

namespace network
{
	class Connection;

	class SSLServer : private QTcpServer
	{
		Q_OBJECT

	private:
		using super = QTcpServer;

		std::unique_ptr<QSslCertificate> m_SSLCertificate;
		std::unique_ptr<QSslKey> m_SSLKey;
		void incomingConnection(int _socketDescriptor) override;

	public:
		void start(const config::Network& _config);
	};
} // namespace network
