#include "stdafx.h"
#include "MessageListener.hpp"
#include "moc_MessageListener.hpp"
#include "NetworkMessage.hpp"
#include <QtSql/QSqlQuery>
#include "Message.pb.h"
#include "DatabaseTables.h"

// free functions
namespace
{
	bool checkValidation(const network::protobuf::GameIdent& _ident)
	{
		using namespace database;
		QSqlQuery query(QString("SELECT ") + TableGames::Field::hash + " FROM " + TableGames::name + " WHERE " + TableGames::Field::id + "=" +
			QString::number(_ident.gameid()), QSqlDatabase::database());
		assert(query.exec());
		if (!query.first())
		{
			LOG_ERR("Invalid game: " + _ident.gameid());
			return false;
		}

		// check the received hash for the game.
		auto check = QByteArray::fromRawData(_ident.hash().data(), _ident.hash().size()) == query.value(0).toByteArray();
		if (!check)
			LOG_ERR("Invalid hash received for game: " + _ident.gameid());
		return check;
	}
} // anonymous namespace

MessageListener::MessageListener(QObject* _parent) :
	super(_parent)
{
}

void MessageListener::onMessageReceived(const network::IMessage& _msg)
{
	if (!_msg.isComplete())
	{
		LOG_ERR("Received message is not complete.");
		return;
	}

	auto& bytes = _msg.getBytes();
	network::protobuf::GameAchievement msg;
	msg.ParseFromArray(bytes.data(), bytes.size());
	auto& ident = msg.gameid();
	if (!checkValidation(ident))
		return;
}
