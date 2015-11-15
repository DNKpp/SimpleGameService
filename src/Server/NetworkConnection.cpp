#include "stdafx.h"
#include "NetworkConnection.hpp"
#include "moc_NetworkConnection.hpp"
#include "NetworkExceptions.h"

namespace network
{
	Connection::Connection(QTcpSocket& _socket, QObject* _parent) :
		super(_parent),
		m_Socket(_socket)
	{
		assert(_parent);
		assert(connect(&m_Socket, SIGNAL(disconnected()), this, SLOT(_onDisconnected())));
		if (m_Socket.state() != QAbstractSocket::ConnectedState)
			_onDisconnected();
		assert(connect(&m_Socket, SIGNAL(readyRead()), this, SLOT(_onReadyRead())));
	}

	void Connection::_onDisconnected()
	{
		m_Socket.deleteLater();
		deleteLater();
	}

	void Connection::_createNewMessage(QByteArray& _buffer)
	{
		m_NewMessage = std::make_unique<IMessage>(this);
		_buffer.remove(0, m_NewMessage->setupHeader(_buffer.constData(), _buffer.size()));

		assert(connect(&m_Socket, SIGNAL(disconnected()), this, SLOT(_onDisconnected())));
		if (m_Socket.state() != QAbstractSocket::ConnectedState)
			m_NewMessage->onConnectionClosed();
	}

	void Connection::_onReadyRead()
	{
		auto buffer = m_PreviousBuffer;
		buffer += m_Socket.readAll();
		m_PreviousBuffer.clear();
		try
		{
			while (!buffer.isEmpty())
			{
				if (!m_NewMessage)
				{
					// after parsing the header, the buffer could be empty, thus we have to check for.
					// If the header is not completely available, an IMessageHeaderError exception is thrown anyway.
					_createNewMessage(buffer);
					if (buffer.isEmpty())
						return;
				}

				buffer.remove(0, m_NewMessage->append(buffer.constData(), buffer.size()));
				if (m_NewMessage->isComplete())
				{
					emit messageComplete(*m_NewMessage);
					m_NewMessage.reset();
				}
			}
		}
		catch (const IMessageHeaderError& e)
		{
			LOG_ERR(e.what());
			m_NewMessage.reset();
			// store buffer for next read
			m_PreviousBuffer = buffer;
		}
		catch (const IMessageError& e)
		{
			LOG_ERR(e.what());
			m_NewMessage.reset();
		}
	}
} // namespace network
