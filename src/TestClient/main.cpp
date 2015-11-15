#include "stdafx.h"

#include "NetworkClient.h"

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);

	NetworkClient client;
	return app.exec();
}

