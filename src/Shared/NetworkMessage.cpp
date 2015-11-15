#include "NetworkMessage.hpp"
#include "moc_NetworkMessage.hpp"
#include <cassert>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>

namespace network
{
	IMessage::IMessage(Connection* _connection, QObject* _parent) :
		super(_parent),
		m_Connection(_connection)
	{
		assert(_connection);
	}

	int IMessage::setupHeader(const char* _c, int _size)
	{
		auto buffer = QByteArray::fromRawData(_c, _size);
		auto index = buffer.indexOf(PacketBegin.data());
		if (index < 0)
			throw IMessageHeaderError("Invalid packet received.");
		QDataStream in(buffer);
		in.skipRawData(index + PacketBegin.size());
		in >> m_ExpectedSize;
		if (m_ExpectedSize <= 0)
			throw IMessageError("Invalid packet size received.");
		m_Bytes.resize(m_ExpectedSize);
		return (int)PacketBegin.size() + SizeBytes;
	}

	int IMessage::append(const char* _c, int _size)
	{
		if (m_ExpectedSize == 0)
			throw IMessageError("Message has not been established.");
		auto buffer = QByteArray::fromRawData(_c, _size);
		QDataStream in(buffer);
		auto bytesRead = in.readRawData(m_Bytes.data() + m_CurrentByte, m_ExpectedSize - m_CurrentByte);
		if (bytesRead < 0)
			throw IMessageError("Error while buffer reading.");
		m_CurrentByte += bytesRead;
		return bytesRead;
	}

	const ByteVector& IMessage::getBytes() const
	{
		return m_Bytes;
	}

	bool IMessage::isComplete() const
	{
		return m_CurrentByte == m_ExpectedSize;
	}

	void IMessage::onConnectionClosed()
	{
		m_Connection = nullptr;
	}
}
