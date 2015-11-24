#pragma once

namespace network
{
	class IMessage : public QObject
	{
		Q_OBJECT
	private:
		using super = QObject;
		QByteArray m_Bytes;
		MessageSizeType m_ExpectedSize = 0;
		MessageType m_MessageType = 0;
		VersionType m_Version = 0;
		std::size_t m_CurrentByte = 0;

	public:
		IMessage(QObject* _parent = nullptr);

		int setupHeader(const char* _c, int _size);
		int append(const char* _c, int _size);

		QByteArray getBytes() const;
		bool isComplete() const;

		MessageType getMessageType() const;
		VersionType getVersion() const;
	};

	class OMessage : public QObject
	{
		Q_OBJECT
	private:
		using super = QObject;
		QByteArray m_Bytes;

	public:
		OMessage(QObject* _parent = nullptr);

		void setup(VersionType _version, MessageType _type, QByteArray _buffer);

		QByteArray getBytes() const;
	};
}
