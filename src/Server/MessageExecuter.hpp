#pragma once

#include "Database.hpp"

namespace network
{
	class IMessage;
}

class MessageExecuter : public QObject, boost::noncopyable
{
	Q_OBJECT
private:
	using super = QObject;

	database::Database m_Database;

public:
	MessageExecuter(QObject* _parent = nullptr);

public slots:
	void onMessageReceived(const network::IMessage&);
};

