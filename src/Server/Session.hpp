#pragma once

namespace network
{
	class Connection;
	class IMessage;
}

namespace task
{
	class TaskWatcher;
}

class Session : public QObject
{
	Q_OBJECT

	friend class task::TaskWatcher;

private:
	using super = QObject;

	static uint64_t SessionCounter;

	network::Connection* m_Connection = nullptr;

	std::atomic<uint64_t> m_GameID = 0;
	std::atomic<uint64_t> m_UserID = 0;

	const uint64_t m_SessionID = ++SessionCounter;
	int m_TaskCounter = 0;

public:
	Session(network::Connection* _con, QObject* _parent = nullptr);

	uint64_t getGameID() const;
	void setGameID(uint64_t _id);
	uint64_t getUserID() const;
	void setUserID(uint64_t _id);
	uint64_t getID() const;

	void sendReply(QByteArray _buffer, uint32_t _type);

private slots:
	void _onDisconnected();
	void _onMessageReceived(const network::IMessage&);
	void _onTaskFinished(QByteArray, uint32_t);
};
