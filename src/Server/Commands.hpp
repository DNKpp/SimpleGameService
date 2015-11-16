#pragma once

#include "stdafx.h"
#include "Messages.pb.h"
#include "DatabaseTables.hpp"
#include "NetworkConnection.hpp"

struct ConnectionInfo
{
	uint64_t connectionID;
	uint64_t userID;
};

namespace commands
{
	template <class Message>
	class Command
	{
	public:
		Command(ConnectionInfo& _info, const Message&)
		{
			static_assert(false, "Not implemented for this type of message.");
		}

		QByteArray work()
		{
			return QByteArray();
		}
	};

	/*#####
	# Authentication
	#####*/
	template <>
	class Command<protobuf::Authentication>
	{
	private:
		using Message = protobuf::Authentication;
		const Message& m_Message;
		ConnectionInfo& m_Connection;

	private:
		protobuf::AuthenticationAnswer _checkPassword()
		{
			using namespace database;
			auto database = QSqlDatabase::database();
			QSqlQuery query(QString("SELECT * FROM ") + TableUsers::name + " WHERE " +
				TableUsers::Field::name + " = '" + QString::fromStdString(m_Message.name()) + "'");
			query.setForwardOnly(true);
			assert(query.exec());

			protobuf::AuthenticationAnswer msg;
			// user not found
			if (!query.first())
			{
				msg.set_result(protobuf::AuthenticationAnswer::unknown);
				return msg;
			}

			// wrong password
			auto password = QByteArray::fromRawData(m_Message.password().data(), m_Message.password().size());
			password = QCryptographicHash::hash(password, QCryptographicHash::Sha256);
			password = QCryptographicHash::hash(query.value(TableUsers::Field::salt).toByteArray() + password, QCryptographicHash::Sha256);
			if (password != query.value(TableUsers::Field::password).toByteArray())
			{
				msg.set_result(protobuf::AuthenticationAnswer::wrongPassword);
				return msg;
			}

			m_Connection.userID = query.value(TableUsers::Field::id).toLongLong();
			return msg;
		}

	public:
		Command(ConnectionInfo& _info, const Message& _msg) :
			m_Message(_msg),
			m_Connection(_info)
		{
		}

		QByteArray work()
		{
			auto msg = _checkPassword();
			QByteArray buffer(msg.ByteSize(), 0);
			msg.SerializeToArray(buffer.data(), buffer.size());
			return buffer;
		}
	};

	/*#####
	# GameAchievement
	#####*/
	template <>
	class Command<protobuf::Achievement>
	{
	private:
		using Message = protobuf::Achievement;
		const Message& m_Message;
		ConnectionInfo& m_Connection;

	private:
		bool _checkGameValidation(const protobuf::GameIdent& _ident)
		{
			using namespace database;
			QSqlQuery query(QString("SELECT ") + TableGames::Field::hash + " FROM " + TableGames::name + " WHERE " + TableGames::Field::id + "=" +
				QString::number(_ident.gameid()), QSqlDatabase::database());
			query.setForwardOnly(true);
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

	public:
		Command(ConnectionInfo& _info, const Message& _msg) :
			m_Message(_msg),
			m_Connection(_info)
		{
		}

		QByteArray work()
		{
			if (m_Connection.userID == 0)
				return QByteArray();
			using namespace database;
			auto& ident = m_Message.gameid();
			if (!_checkGameValidation(ident))
				return QByteArray();

			std::unordered_set<uint64_t> IDs;

			// check if achievement exists and belongs to the game
			QString queryString = QString("SELECT ") + TableAchievements::Field::id + " FROM " + TableAchievements::name + " WHERE ";
			for (int i = 0; i < m_Message.achievements_size(); ++i)
			{
				auto& achieve = m_Message.achievements(i);
				IDs.insert(achieve.id());
				queryString += QString("(") + TableAchievements::Field::id + "=" + QString::number(achieve.id()) + " AND " +
					TableAchievements::Field::gameID + "=" + QString::number(ident.gameid()) + ") ";
				if (i < m_Message.achievements_size() - 1)
					queryString += " OR ";
			}
			QSqlQuery query(queryString, QSqlDatabase::database());
			query.setForwardOnly(true);
			assert(query.exec());
			if (query.size() <= 0)
				return QByteArray();

			// check if user has achievement currently
			queryString = QString("SELECT ") + TableUserAchievements::Field::achievementID + " FROM " + TableUserAchievements::name + " WHERE ";
			while (query.next())
			{
				auto id = query.value(0).toULongLong();
				queryString += QString("(") + TableUserAchievements::Field::achievementID + "=" + QString::number(id) + " AND " +
					TableUserAchievements::Field::userID + "=" + QString::number(1) + ")OR";					// remove test value
			}
			queryString.chop(2);
			assert(query.exec(queryString));
			while (query.next())
				IDs.erase(query.value(0).toULongLong());

			if (IDs.empty())
				return QByteArray();

			// insert new achievements
			// ToDo: Send achievement IDs back to the sender.
			queryString = QString("INSERT INTO ") + TableUserAchievements::name + " (" +
				TableUserAchievements::Field::achievementID + ", " + TableUserAchievements::Field::userID + ") VALUES ";
			std::string logString;
			for (auto& id : IDs)
			{
				queryString += QString("(") + QString::number(id) + ", " + QString::number(1) + "),";		// remove test value
				logString += std::to_string(id) + ", ";
			}
			logString.resize(logString.size() - 2);
			queryString.chop(1);
			assert(query.exec(queryString));
			LOG_INFO("Added user achievements for userID: " + 1 + " achievements: " + logString);		// remove test value
			return QByteArray();
		}
	};

	enum class Type
	{
		authentication,
		achievement,
	};

	namespace
	{
		template <class Message>
		QByteArray _doCommand(Message _msg, QByteArray _buffer, ConnectionInfo& _con)
		{
			_msg.ParseFromArray(_buffer.data(), _buffer.size());
			Command<Message> cmd(_con, _msg);
			return cmd.work();
		}
	} // anonymous namespace

	QByteArray doCommand(Type _type, QByteArray _buffer, ConnectionInfo& _con)
	{
		using namespace protobuf;
		switch (_type)
		{
		case Type::authentication: return _doCommand(Authentication(), _buffer, _con);
		case Type::achievement: return _doCommand(Achievement(), _buffer, _con);
		}
		return QByteArray();
	}
} // namespace commands
