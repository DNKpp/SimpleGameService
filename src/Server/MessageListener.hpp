#pragma once

namespace network
{
	class IMessage;
}

class MessageListener : public QObject, boost::noncopyable
{
	Q_OBJECT
private:
	using super = QObject;

public:
	MessageListener(QObject* _parent = nullptr);

public slots:
	void onMessageReceived(const network::IMessage&);
};

