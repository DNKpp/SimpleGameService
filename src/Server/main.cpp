#include "stdafx.h"
#include "Server.hpp"
#include "NetworkExceptions.h"
//
int main(int argc, char* argv[])
{
	// init log
	GlobalLog.addOutput(std::make_unique<sl::log::ConsoleOutput>());

	QCoreApplication app(argc, argv);
	Server svr;

	try
	{
		svr.start();
	}
	catch (const network::InitError& e)
	{
		LOG_CRITIC(e.what());
	}
	return app.exec();
}
