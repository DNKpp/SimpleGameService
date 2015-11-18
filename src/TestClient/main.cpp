#include "stdafx.h"
#include "NetworkClient.hpp"
#include "ReplyHandler.hpp"

int main(int argc, char* argv[])
{
	GlobalLog.addOutput(std::make_unique<sl::log::ConsoleOutput>());
	GlobalLog.addOutput(std::make_unique<sl::log::HTMLFileOutput>("logfile"));
	QCoreApplication app(argc, argv);

	reply::Handler replyHandler;
	network::Client client;
	assert(QObject::connect(&client, SIGNAL(messageReceived(const network::IMessage&)), &replyHandler, SLOT(_onMessageReceived(const network::IMessage&))));
	return app.exec();
}

