#include "stdafx.h"
#include "NetworkConnection.hpp"
#include "moc_NetworkConnection.hpp"
#include "NetworkExceptions.h"

namespace
{
	QByteArray _setupMsg(QByteArray _msg, uint32_t _type)
	{
		QByteArray buffer;
		QDataStream out(&buffer, QIODevice::WriteOnly);
		out.writeRawData(network::PacketBegin.data(), network::PacketBegin.size());
		out << (uint16_t)1;	// version
		out << (uint16_t)_type;
		out << (uint32_t)_msg.size();
		buffer += _msg;
		return buffer;
	}

} // anonymous namespace

namespace network
{
	uint64_t Connection::currentConnectionID = 0;
	Connection::Connection(QTcpSocket& _socket, QObject* _parent) :
		super(_parent),
		m_Socket(_socket),
		m_ConnectionID(++currentConnectionID)	// make connectionID unique per server runtime
	{
		assert(_parent);
		assert(connect(&m_Socket, SIGNAL(disconnected()), this, SLOT(_onDisconnected())));
		if (m_Socket.state() != QAbstractSocket::ConnectedState)
			_onDisconnected();
		assert(connect(&m_Socket, SIGNAL(readyRead()), this, SLOT(_onReadyRead())));
		assert(connect(&m_Socket, SIGNAL(bytesWritten(qint64)), this, SLOT(_onBytesWritten(qint64))));
	}

	void Connection::_createNewMessage(QByteArray& _buffer)
	{
		m_NewMessage = std::make_unique<IMessage>(getConnectionID());
		_buffer.remove(0, m_NewMessage->setupHeader(_buffer.constData(), _buffer.size()));
	}

	void Connection::send(QByteArray _msg, uint32_t _type)
	{
		bool empty = m_OutBuffers.empty();
		_msg = _setupMsg(_msg, _type);
		m_OutBuffers.push(_msg);
		// send only if buffer is empty, thus we get sure, we do not mix two messages
		if (empty)
			m_Socket.write(_msg.data(), _msg.size());
	}

	void Connection::authenticatedAs(uint64_t _ID)
	{
		m_UserID = _ID;
	}

	bool Connection::isAuthenticated() const
	{
		return m_UserID != 0;
	}

	uint64_t Connection::getUserID() const
	{
		return m_UserID;
	}

	uint64_t Connection::getConnectionID() const
	{
		return m_ConnectionID;
	}

	void Connection::_onReadyRead()
	{
		auto buffer = m_PreviousBuffer;
		auto newBuffer = m_Socket.readAll();
		LOG_INFO("Received " + newBuffer.size() + " bytes from host: " + m_Socket.peerAddress().toString() + " port: " + m_Socket.peerPort());
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
		auto& buffer = m_OutBuffers.front();
		if (buffer.size() == _bytes)
		{
			m_OutBuffers.pop();
			if (!m_OutBuffers.empty())
				m_Socket.write(m_OutBuffers.front().data(), m_OutBuffers.front().size());
		}
		else
			buffer.remove(0, _bytes);
	}

	void Connection::_onDisconnected()
	{
		emit disconnected(*this);
		m_Socket.deleteLater();
		deleteLater();
	}
} // namespace network
