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
	QFuture<Result> run(QByteArray _buffer, Session& _session, std::unique_ptr<Base>& _task)
	{
		auto index = ++TaskCounter;
		_task = std::make_unique<Task>(_buffer, _session, index);
		return QtConcurrent::run(std::bind(&Task::start, _task.get()));
	}

	QFuture<Result> run(QByteArray _buffer, Session& _session, uint32_t _type, std::unique_ptr<Base>& _task)
	{
		switch (_type)
		{
		case network::svr::authentication: return run<Authenticate>(_buffer, _session, _task);
		case network::svr::login: return run<Login>(_buffer, _session, _task);
		case network::svr::achievement: return run<Achievement>(_buffer, _session, _task);
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
		m_Future = task::run(_buffer, m_Session, id, m_Task);
		m_Watcher.setFuture(m_Future);
	}

	void TaskWatcher::_onFinished()
	{
		auto result = m_Watcher.result();
		emit replyCreated(result.first, result.second);
		deleteLater();
	}

	/*#####
	# Base
	#####*/
	Base::Base(QByteArray _buffer, Session& _session, uint64_t _index) :
		m_Buffer(_buffer),
		m_Session(_session),
		m_Index(_index)
	{
		m_Database = QSqlDatabase::cloneDatabase(QSqlDatabase::database("DB"), "DB" + QString::number(m_Index));
	}

	Base::~Base()
	{
		if (m_Database.isOpen())
		{
			m_Database.close();
			m_Database = QSqlDatabase();
		}
		QSqlDatabase::removeDatabase("DB" + QString::number(m_Index));
	}

	Result Base::start()
	{
		Result result;
		do
		{
			try
			{
				m_Database.open();
			}
			catch (const std::runtime_error& e)
			{
				LOG_ERR(e.what());
				continue;
			}
			catch (...)
			{
				LOG_ERR("Session: " + m_Session.getID() + " unknown error while opening MySQL database.");
				continue;
			}
			break;
		} while (!m_Database.isOpen());

		return run();
	}

	/*#####
	# Authenticate
	#####*/
	Authenticate::Authenticate(QByteArray _buffer, Session& _session, uint64_t _index) :
		Base(_buffer, _session, _index)
	{
	}

	bool Authenticate::_authGame()
	{
		protobuf::Authentication msg;
		msg.ParseFromArray(m_Buffer.data(), m_Buffer.size());

		using namespace database;
		QSqlQuery query(m_Database);
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
	Login::Login(QByteArray _buffer, Session& _session, uint64_t _index) :
		Base(_buffer, _session, _index)
	{
	}

	bool Login::_tryLogin()
	{
		protobuf::Login msg;
		msg.ParseFromArray(m_Buffer.data(), m_Buffer.size());

		using namespace database;
		QSqlQuery query(m_Database);
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

	/*#####
	# Achievement
	#####*/
	Achievement::Achievement(QByteArray _buffer, Session& _session, uint64_t _index) :
		Base(_buffer, _session, _index)
	{
	}

	protobuf::AchievementReply Achievement::_check()
	{
		protobuf::Achievement msg;
		protobuf::AchievementReply reply;
		msg.ParseFromArray(m_Buffer.data(), m_Buffer.size());
		auto achievementID = msg.id();
		reply.set_id(achievementID);
		reply.set_result(protobuf::AchievementReply_Type_failed);

		auto userID = m_Session.getUserID();
		auto gameID = m_Session.getGameID();
		if (gameID == 0 || userID == 0)
			return reply;

		using namespace database;
		std::unordered_set<uint64_t> IDs;

		// check if achievement exists and belongs to the game
		QSqlQuery query(m_Database);
		query.setForwardOnly(true);
		assert(query.prepare(QString("SELECT * FROM ") + TableAchievements::name + " WHERE " + TableAchievements::Field::id + "= ? AND " +
			TableAchievements::Field::gameID + "= ?"));
		query.addBindValue(achievementID);
		query.addBindValue(gameID);
		assert(query.exec());
		if (!query.first())
			return reply;

		// check if user has achievement currently
		assert(query.prepare(QString("SELECT ") + TableUserAchievements::Field::achievementID + " FROM " + TableUserAchievements::name + " WHERE " +
			TableUserAchievements::Field::achievementID + "= ? AND " + TableUserAchievements::Field::userID + "= ?"));
		query.addBindValue(achievementID);
		query.addBindValue(userID);
		assert(query.exec());
		if (query.first())
		{
			reply.set_result(protobuf::AchievementReply_Type_exists);
			return reply;
		}
		
		// insert new achievements
		assert(query.prepare(QString("INSERT INTO ") + TableUserAchievements::name + " (" +
			TableUserAchievements::Field::achievementID + ", " + TableUserAchievements::Field::userID + ") VALUES (?, ?)"));
		query.addBindValue(achievementID);
		query.addBindValue(userID);
		assert(query.exec());
		reply.set_result(protobuf::AchievementReply_Type_complete);
		LOG_INFO("Add user achievement for userID: " + userID + " achievement: " + achievementID);
		return reply;
	}

	Result Achievement::run()
	{
		auto reply = _check();
		Result result;
		result.first.resize(reply.ByteSize());
		result.second = network::client::achievementReply;
		reply.SerializePartialToArray(result.first.data(), result.first.size());
		return result;
	}
} // namespace task
