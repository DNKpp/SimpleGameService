#pragma once

#include "NetworkMessage.hpp"

namespace network
{
	class Connection : public QObject, boost::noncopyable
	{
		Q_OBJECT

	private:
		static uint64_t currentConnectionID;

		using super = QObject;
		QTcpSocket& m_Socket;
		std::unique_ptr<IMessage> m_NewMessage;
		std::queue<QByteArray> m_OutBuffers;
		const uint64_t m_ConnectionID;

		QByteArray m_PreviousBuffer;	// If an exception is thrown, because of invalid header, it could be partial available. Because of this, we store the
										// buffer here, if an IMessageHeaderError exception was thrown.

		// server only
		uint64_t m_UserID = 0;

	private:
		void _createNewMessage(QByteArray& _buffer);

	public:
		Connection(QTcpSocket& _socket, QObject* _parent);

		void close();
		void send(QByteArray _msg, uint32_t _type);

		void authenticatedAs(uint64_t _ID);
		bool isAuthenticated() const;
		uint64_t getUserID() const;
		uint64_t getConnectionID() const;

	signals:
		void messageReceived(const network::IMessage&);
		void disconnected(const Connection&);

	private slots:
		void _onDisconnected();
		void _onReadyRead();
		void _onBytesWritten(qint64 _bytes);
	};
}
