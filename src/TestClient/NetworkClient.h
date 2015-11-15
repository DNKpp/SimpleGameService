#pragma once

class NetworkClient : public QObject
{
	Q_OBJECT
private:
	using super = QObject;
	QTcpSocket* m_Socket = nullptr;

	QByteArray m_Message;
	void _createMsg();

public:
	NetworkClient();

private slots:
	void _onConnected();
	void _onBytesWritten(qint64 _bytes);
};

