#pragma once

#include "NetworkMessage.hpp"

namespace network
{
	class Connection : public QObject
	{
		Q_OBJECT

	private:
		using super = QObject;
		QSslSocket& m_Socket;
		std::unique_ptr<IMessage> m_NewMessage;

		QByteArray m_PreviousBuffer;	// If an exception is thrown, because of invalid header, it could be partial available. Because of this, we store the
										// buffer here, if an IMessageHeaderError exception was thrown.

	private:
		void _createNewMessage(QByteArray& _buffer);

	public:
		Connection(QSslSocket& _socket, QObject* _parent);

		void close();

	signals:
		void messageReceived(const network::IMessage&);

	private slots:
		void _onDisconnected();
		void _onReadyRead();
		void _onBytesWritten(qint64 _bytes);
		void _onSocketReady();
		void _onSSLErrors(const QList<QSslError>&);

	public slots:
		void onPacketSent(const network::OMessage&);
	};
}
