#pragma once

#include "NetworkExceptions.h"

class Session;

namespace network
{
	static const std::string PacketBegin{ "simpledata" };

	class IMessage : public QObject
	{
		Q_OBJECT
	private:
		using super = QObject;
		QByteArray m_Bytes;
		uint32_t m_ExpectedSize = 0;
		uint16_t m_MessageType = 0;
		uint16_t m_Version = 0;
		std::size_t m_CurrentByte = 0;

		// Server only
		Session* m_Session = nullptr;

	public:
		IMessage(QObject* _parent = nullptr);

		int setupHeader(const char* _c, int _size);
		int append(const char* _c, int _size);

		QByteArray getBytes() const;
		bool isComplete() const;

		uint16_t getMessageType() const;
		uint16_t getVersion() const;

		Session* getSession() const;
		void setSession(Session* _session);
	};
}
