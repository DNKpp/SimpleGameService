#pragma once

#include <vector>
#include <stdint.h>
#include <QtCore/QObject>
#include "NetworkExceptions.h"

namespace network
{
	static const std::string PacketBegin{ "simpledata" };
	static const std::size_t SizeBytes = 4;

	class Connection;

	using ByteVector = std::vector<char>;
	class IMessage : public QObject
	{
		Q_OBJECT
	private:
		using super = QObject;
		ByteVector m_Bytes;
		uint32_t m_ExpectedSize = 0;
		std::size_t m_CurrentByte = 0;
		Connection* m_Connection = nullptr;

	public:
		IMessage(Connection* _connection, QObject* _parent = nullptr);

		int setupHeader(const char* _c, int _size);
		int append(const char* _c, int _size);

		const ByteVector& getBytes() const;
		bool isComplete() const;

	public slots:
		void onConnectionClosed();
	};
}
