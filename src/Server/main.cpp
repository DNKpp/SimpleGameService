#include "stdafx.h"
#include "Server.hpp"
//
int main(int argc, char* argv[])
{
	// init log
	GlobalLog.addOutput(std::make_unique<sl::log::ConsoleOutput>());
	GlobalLog.addOutput(std::make_unique<sl::log::HTMLFileOutput>("logfile"));

	//qRegisterMetaType<uint32_t>("uint32_t");

	QCoreApplication app(argc, argv);
	app.setLibraryPaths(QStringList{ "." });
	Server svr;

	try
	{
		svr.start();
		return app.exec();
	}
	catch (const std::runtime_error& e)
	{
		LOG_CRITIC(e.what());
		std::cin.ignore();
	}
}
