#include "stdafx.h"

#include "NetworkClient.h"

int main(int argc, char* argv[])
{
	GlobalLog.addOutput(std::make_unique<sl::log::ConsoleOutput>());
	GlobalLog.addOutput(std::make_unique<sl::log::HTMLFileOutput>("logfile"));
	QCoreApplication app(argc, argv);

	NetworkClient client;
	return app.exec();
}

