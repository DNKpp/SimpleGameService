#pragma once

namespace network
{
	class Connection;
}

class NetworkClient : public QObject
{
	Q_OBJECT
private:
	using super = QObject;
	QTcpSocket* m_Socket = nullptr;
	network::Connection* m_Connection = nullptr;

	QByteArray m_Message;
	void _createMsg();

public:
	NetworkClient();

private slots:
	void _onConnected();
};

