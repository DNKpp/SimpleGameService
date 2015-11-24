#pragma once

#include "Session.hpp"

namespace network
{
	class Connection;
	class IMessage;

	class Client : public QObject
	{
		Q_OBJECT
	private:
		using super = QObject;
		QSslSocket* m_Socket = nullptr;

		Session m_Session;

		QByteArray m_Message;
		void _createMsg();

	public:
		Client();

	private slots:
		void _onConnected();
		void _onSessionReady();
	};
}
