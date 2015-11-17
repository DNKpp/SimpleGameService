#include "stdafx.h"
#include "Tasks.h"
#include "DatabaseTables.hpp"
#include "Messages.pb.h"
#include "MessageTypes.hpp"
#include "Session.h"

namespace task
{
	// factory
	std::unique_ptr<QRunnable> createTask(QByteArray _buffer, Session& _session, const database::Database& _database, uint32_t _type)
	{
		std::unique_ptr<QRunnable> result;

		using namespace network::svr;
		switch (_type)
		{
		case authentication:
			result = std::make_unique<Authenticate>(_buffer, _session, _database);
			break;
		case login:
			result = std::make_unique<Login>(_buffer, _session, _database);
			break;
		case achievement:
			result = std::make_unique<Achievement>(_buffer, _session, _database);
			break;
		default:
			LOG_ERR("Invalid task type.");
		}
		return result;
	}

	/*#####
	# Base
	#####*/
	Base::Base(QByteArray _buffer, Session& _session, const database::Database& _database) :
		m_Database(_database),
		m_Session(_session)
	{
	}

	Session& Base::getSession() const
	{
		return m_Session;
	}

	database::Database& Base::getDatabase()
	{
		return m_Database;
	}

	QByteArray Base::getBuffer() const
	{
		return m_Buffer;
	}

	/*#####
	# Authenticate
	#####*/
	Authenticate::Authenticate(QByteArray _buffer, Session& _session, const database::Database& _database) :
		super(_buffer, _session, _database)
	{
	}

	void Authenticate::run()
	{
		try
		{
			getDatabase().open();
		}
		catch (const std::runtime_error& e)
		{
			LOG_ERR(e.what());
			return;
		}

		protobuf::Authentication msg;
		msg.ParseFromArray(getBuffer().data(), getBuffer().size());

		using namespace database;
		QSqlQuery query;
		(QString("SELECT ") + TableGames::Field::hash + " FROM " + TableGames::name + " WHERE " + TableGames::Field::id + "=" +
			QString::number(msg.id()), getDatabase().getDatabase());
		query.setForwardOnly(true);
		assert(query.exec());
		if (!query.first())
		{
			LOG_ERR("Invalid game: " + msg.id());
			return;
		}

		// check the received hash for the game.
		if (QByteArray::fromRawData(msg.hash().data(), msg.hash().size()) != query.value(0).toByteArray())
		{
			LOG_ERR("Invalid hash received for game: " + msg.id());
			return;
		}

		getSession().setGameID(msg.id());
		// ToDo: create reply			
	}

	/*#####
	# Login
	#####*/
	Login::Login(QByteArray _buffer, Session& _session, const database::Database& _database) :
		super(_buffer, _session, _database)
	{
	}

	void Login::run()
	{
		try
		{
			getDatabase().open();
		}
		catch (const std::runtime_error& e)
		{
			LOG_ERR(e.what());
			return;
		}

	}

	/*#####
	# Achievement
	#####*/
	Achievement::Achievement(QByteArray _buffer, Session& _session, const database::Database& _database) :
		super(_buffer, _session, _database)
	{
	}

	void Achievement::run()
	{
		try
		{
			getDatabase().open();
		}
		catch (const std::runtime_error& e)
		{
			LOG_ERR(e.what());
			return;
		}

	}
} // namespace task
