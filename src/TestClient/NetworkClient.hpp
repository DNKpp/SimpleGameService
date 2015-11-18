#pragma once

namespace network
{
	class Connection;
	class IMessage;

	class Client : public QObject
	{
		Q_OBJECT
	private:
		using super = QObject;
		QTcpSocket* m_Socket = nullptr;
		network::Connection* m_Connection = nullptr;

		QByteArray m_Message;
		void _createMsg();

	public:
		Client();

	signals:
		void messageReceived(const network::IMessage&);

	private slots:
		void _onConnected();
		void _onMessageReceived(const network::IMessage&);
	};
}
