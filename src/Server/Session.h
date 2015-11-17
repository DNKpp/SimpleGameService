#pragma once

namespace network
{
	class Connection;
}

class Session : public QObject
{
	Q_OBJECT

	using super = QObject;
private:
	network::Connection* m_Connection = nullptr;
	QMutex m_ConnectionMutex;

	std::atomic<uint64_t> m_GameID = 0;
	std::atomic<uint64_t> m_UserID = 0;

public:
	Session(network::Connection* _con, QObject* _parent = nullptr);

	uint64_t getGameID() const;
	void setGameID(uint64_t _id);
	uint64_t getUserID() const;
	void setUserID(uint64_t _id);

	void sendReply(QByteArray _buffer, uint32_t _type);

private slots:
	void _onDisconnected();
};

