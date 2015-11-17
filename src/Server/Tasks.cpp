#include "stdafx.h"
#include "Tasks.hpp"
#include "moc_Tasks.hpp"
#include "DatabaseTables.hpp"
#include "Messages.pb.h"
#include "MessageTypes.hpp"
#include "Session.hpp"

namespace task
{
	// we need this counter to name all database connections unique
	static uint64_t TaskCounter = 0;
	/*#####
	# TaskWatcher
	#####*/
	template <class Task>
	QFuture<Result> run(QByteArray _buffer, Session& _session)
	{
		Task task(_buffer, _session);
		return QtConcurrent::run(task, &Task::run);
	}

	QFuture<Result> run(QByteArray _buffer, Session& _session, uint32_t _type)
	{
		++TaskCounter;
		switch (_type)
		{
		case network::svr::authentication: return run<Authenticate>(_buffer, _session);
		case network::svr::login: return run<Login>(_buffer, _session);
		default:
			throw std::runtime_error("Unknown task type.");
		}
	}

	TaskWatcher::TaskWatcher(Session& _session, QObject* _parent) :
		super(_parent),
		m_Session(_session)
	{
		assert(_parent);
		++m_Session.m_TaskCounter;
		assert(connect(this, SIGNAL(replyCreated(QByteArray, uint32_t)), &m_Session, SLOT(_onTaskFinished(QByteArray, uint32_t))));
	}

	TaskWatcher::~TaskWatcher()
	{
		m_Future.waitForFinished();
		++m_Session.m_TaskCounter;
	}

	void TaskWatcher::run(QByteArray _buffer, uint32_t id)
	{
		assert(connect(&m_Watcher, SIGNAL(finished()), this, SLOT(_onFinished())));
		m_Future = task::run(_buffer, m_Session, id);
		m_Watcher.setFuture(m_Future);
	}

	void TaskWatcher::_onFinished()
	{
		auto result = m_Watcher.result();
		emit replyCreated(result.first, result.second);
		deleteLater();
	}

	/*#####
	# Authenticate
	#####*/
	Authenticate::Authenticate(QByteArray _buffer, Session& _session) :
		m_Buffer(_buffer),
		m_Session(_session)
	{
	}

	bool Authenticate::_authGame()
	{
		auto database = QSqlDatabase::cloneDatabase(QSqlDatabase::database("DB"), QString::number(TaskCounter));
		try
		{
			database.open();
		}
		catch (const std::runtime_error& e)
		{
			LOG_ERR(e.what());
			return false;
		}

		protobuf::Authentication msg;
		msg.ParseFromArray(m_Buffer.data(), m_Buffer.size());

		using namespace database;
		QSqlQuery query(database);
		query.setForwardOnly(true);
		assert(query.exec(QString("SELECT ") + TableGames::Field::hash + " FROM " + TableGames::name + " WHERE " + TableGames::Field::id + "=" + QString::number(msg.id())));
		if (!query.first())
		{
			LOG_ERR("Session: " + m_Session.getID() + " Invalid game : " + msg.id());
			return false;
		}

		// check the received hash for the game.
		auto received = QByteArray::fromRawData(msg.hash().data(), msg.hash().size());
		auto game = query.value(0).toByteArray();
		if (received != game)
		{
			qDebug() << received;
			qDebug() << game << "\n";
			LOG_ERR("Session: " + m_Session.getID() + " Invalid hash received for game: " + msg.id());
			return false;
		}

		m_Session.setGameID(msg.id());
		LOG_INFO("Session: " + m_Session.getID() + " Authenticated for game: " + msg.id());
		return true;
	}

	Result Authenticate::run()
	{
		protobuf::AuthenticationReply reply;
		reply.set_result(_authGame());
		Result result;
		result.first.resize(reply.ByteSize());
		result.second = network::client::authenticationReply;
		reply.SerializePartialToArray(result.first.data(), result.first.size());
		return result;
	}

	/*#####
	# Login
	#####*/
	Login::Login(QByteArray _buffer, Session& _session) :
		m_Buffer(_buffer),
		m_Session(_session)
	{
	}

	bool Login::_tryLogin()
	{
		auto database = QSqlDatabase::cloneDatabase(QSqlDatabase::database("DB"), QString::number(TaskCounter));
		try
		{
			database.open();
		}
		catch (const std::runtime_error& e)
		{
			LOG_ERR(e.what());
			return false;
		}

		protobuf::Login msg;
		msg.ParseFromArray(m_Buffer.data(), m_Buffer.size());

		using namespace database;
		QSqlQuery query(database);
		query.setForwardOnly(true);
		assert(query.exec(QString("SELECT * FROM ") + TableUsers::name + " WHERE " +
			TableUsers::Field::name + " = '" + QString::fromStdString(msg.name()) + "'"));

		// user not found
		if (!query.first())
		{
			LOG_ERR("Session: " + m_Session.getID() + " User not found Name:" + msg.name());
			return false;
		}

		auto userID = query.value(TableUsers::Field::id).toLongLong();

		// wrong password
		auto password = QByteArray::fromRawData(msg.password().data(), msg.password().size());
		// hash the received values, after that put salt in front and hash it again. That is the password hash stored in DB.
		password = QCryptographicHash::hash(password, QCryptographicHash::Sha256);
		password = QCryptographicHash::hash(query.value(TableUsers::Field::salt).toByteArray() + password, QCryptographicHash::Sha256);
		if (password != query.value(TableUsers::Field::password).toByteArray())
		{
			LOG_ERR("Session: " + m_Session.getID() + " Incorrect password for user ID: " + userID);
			return false;
		}

		m_Session.setUserID(userID);
		LOG_INFO("Session: " + m_Session.getID() + " Login as ID: " + userID);
		return true;
	}

	Result Login::run()
	{
		protobuf::LoginReply reply;
		reply.set_result(_tryLogin());
		Result result;
		result.first.resize(reply.ByteSize());
		result.second = network::client::loginReply;
		reply.SerializePartialToArray(result.first.data(), result.first.size());
		return result;
	}

	///*#####
	//# Achievement
	//#####*/
	//Result Achievement::run()
	//{
	//	try
	//	{
	//		_database.open();
	//	}
	//	catch (const std::runtime_error& e)
	//	{
	//		LOG_ERR(e.what());
	//		return Result();
	//	}
	//	return Result();
	//}
} // namespace task
