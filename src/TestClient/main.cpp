#include "stdafx.h"
#include "NetworkClient.hpp"
#include "Session.hpp"

int main(int argc, char* argv[])
{
	GlobalLog.addOutput(std::make_unique<sl::log::ConsoleOutput>());
	GlobalLog.addOutput(std::make_unique<sl::log::HTMLFileOutput>("logfile"));
	QCoreApplication app(argc, argv);

	Session session;
	network::Client client;
	assert(QObject::connect(&client, SIGNAL(messageReceived(const network::IMessage&)), &session, SLOT(_onMessageReceived(const network::IMessage&))));
	return app.exec();
}

