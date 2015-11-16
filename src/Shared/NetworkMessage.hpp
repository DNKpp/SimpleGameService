#pragma once

#include <vector>
#include <stdint.h>
#include <QtCore/QObject>
#include "NetworkExceptions.h"

namespace network
{
	static const std::string PacketBegin{ "simpledata" };

	using ByteVector = std::vector<char>;
	class IMessage : public QObject
	{
		Q_OBJECT
	private:
		using super = QObject;
		ByteVector m_Bytes;
		uint32_t m_ExpectedSize = 0;
		uint16_t m_MessageType = 0;
		uint16_t m_Version = 0;
		std::size_t m_CurrentByte = 0;
		uint64_t m_ConnectionID = 0;

	public:
		IMessage(uint64_t _conID, QObject* _parent = nullptr);

		int setupHeader(const char* _c, int _size);
		int append(const char* _c, int _size);

		const ByteVector& getBytes() const;
		bool isComplete() const;

		uint16_t getMessageType() const;
		uint16_t getVersion() const;

		uint64_t getConnectionID() const;
	};
}
