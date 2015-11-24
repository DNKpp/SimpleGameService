#include "stdafx.h"
#include "NetworkConnection.hpp"
#include "moc_NetworkConnection.hpp"
#include "NetworkExceptions.hpp"

namespace network
{
	Connection::Connection(QSslSocket& _socket, QObject* _parent) :
		super(_parent),
		m_Socket(_socket)
	{
		assert(_parent);
		connect(&m_Socket, SIGNAL(encrypted()), this, SLOT(_onSocketReady()));
		connect(&m_Socket, SIGNAL(disconnected()), this, SLOT(_onDisconnected()));
		connect(&m_Socket, SIGNAL(readyRead()), this, SLOT(_onReadyRead()));
		connect(&m_Socket, SIGNAL(bytesWritten(qint64)), this, SLOT(_onBytesWritten(qint64)));
	}

	void Connection::_createNewMessage(QByteArray& _buffer)
	{
		m_NewMessage = std::make_unique<IMessage>();
		_buffer.remove(0, m_NewMessage->setupHeader(_buffer.constData(), _buffer.size()));
	}

	void Connection::onPacketSent(const network::OMessage& _msg)
	{
		m_Socket.write(_msg.getBytes());
	}

	void Connection::_onReadyRead()
	{
		auto buffer = m_PreviousBuffer;
		auto newBuffer = m_Socket.readAll();
		LOG_INFO("Received " + newBuffer.size() + " bytes Host: " + m_Socket.peerAddress().toString() + " port: " + m_Socket.peerPort());
		buffer += newBuffer;
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
					emit messageReceived(*m_NewMessage);
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

	void Connection::_onBytesWritten(qint64 _bytes)
	{
		LOG_DEBUG("Sent " + _bytes + " bytes Host: " + m_Socket.peerAddress().toString() + " Port: " + m_Socket.peerPort());
	}

	void Connection::_onSocketReady()
	{
		int x = 0;
	}

	void Connection::_onSSLErrors(const QList<QSslError>& _errors)
	{
		for (auto& error : _errors)
			LOG_ERR(error.errorString().toStdString());
	}

	void Connection::_onDisconnected()
	{
		m_Socket.deleteLater();
		deleteLater();
	}
} // namespace network
