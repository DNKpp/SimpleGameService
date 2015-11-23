#include "stdafx.h"
#include "NetworkClient.hpp"
#include "Session.hpp"

int main(int argc, char* argv[])
{
	GlobalLog.addOutput(std::make_unique<sl::log::ConsoleOutput>());
	GlobalLog.addOutput(std::make_unique<sl::log::HTMLFileOutput>("logfile"));
	QCoreApplication app(argc, argv);

	network::Client client;
	return app.exec();
}

