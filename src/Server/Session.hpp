#pragma once

namespace network
{
	class IMessage;
	class OMessage;
}

namespace task
{
	class TaskWatcher;
}

class Session : public QObject
{
	Q_OBJECT
private:
	using super = QObject;

	static uint64_t SessionCounter;

	std::atomic<uint64_t> m_GameID = 0;
	std::atomic<uint64_t> m_UserID = 0;

	const uint64_t m_SessionID = ++SessionCounter;
	int m_TaskCounter = 0;

	bool m_Disconnected = false;

	void _deleteSelfIfNecessary();

public:
	Session(QObject* _parent = nullptr);

	uint64_t getGameID() const;
	void setGameID(uint64_t _id);
	uint64_t getUserID() const;
	void setUserID(uint64_t _id);
	uint64_t getID() const;

	void sendPacket(QByteArray _buffer, network::MessageType _type);

signals:
	void packetReady(const network::OMessage&);

private slots:
	void _onDisconnected();
	void _onMessageReceived(const network::IMessage&);
	void _onTaskFinished(QByteArray, network::MessageType);
};

