#include "stdafx.h"
#include "Tasks.hpp"
#include "moc_Tasks.hpp"
#include "DatabaseTables.hpp"
#include "Messages.pb.h"
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

	QFuture<Result> run(QByteArray _buffer, Session& _session, network::MessageType _type, std::unique_ptr<Base>& _task)
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
		connect(this, SIGNAL(replyCreated(QByteArray, network::MessageType)), &m_Session, SLOT(_onTaskFinished(QByteArray, network::MessageType)));
	}

	TaskWatcher::~TaskWatcher()
	{
		m_Future.waitForFinished();
	}

	void TaskWatcher::run(QByteArray _buffer, network::MessageType id)
	{
		connect(&m_Watcher, SIGNAL(finished()), this, SLOT(_onFinished()));
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
				LOG_ERR("Session: " + m_Session.getID() + " unknown error while opening MySQL database connection.");
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
		if (!query.exec(QString("SELECT ") + TableGames::Field::hash + " FROM " + TableGames::name + " WHERE " + TableGames::Field::id + "=" + QString::number(msg.id())))
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}

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
		if (!query.exec(QString("SELECT * FROM ") + TableUsers::name + " WHERE " +
			TableUsers::Field::name + " = '" + QString::fromStdString(msg.name()) + "'"))
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}

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

	bool Achievement::_achievementBelongsToGame(uint64_t _achievementID, uint64_t _gameID)
	{
		using namespace database;
		// check if achievement exists and belongs to the game
		QSqlQuery query(m_Database);
		query.setForwardOnly(true);
		if (!query.prepare(QString("SELECT * FROM ") + TableAchievements::name + " WHERE " + TableAchievements::Field::id + "= ? AND " +
			TableAchievements::Field::gameID + "= ?"))
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}
		query.addBindValue(_achievementID);
		query.addBindValue(_gameID);
		if (!query.exec())
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}
		return query.first();
	}

	bool Achievement::_hasAchievement(uint64_t _achievementID, uint64_t _userID)
	{
		using namespace database;
		// check if user has achievement currently
		QSqlQuery query(m_Database);
		query.setForwardOnly(true);
		if (!query.prepare(QString("SELECT * FROM ") + TableUserAchievements::name + " WHERE " +
			TableUserAchievements::Field::achievementID + "= ? AND " + TableUserAchievements::Field::userID + "= ?"))
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}
		query.addBindValue(_achievementID);
		query.addBindValue(_userID);
		if (!query.exec())
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}
		return query.first();
	}

	void Achievement::_insertAchievement(uint64_t _achievementID, uint64_t _userID)
	{
		using namespace database;
		// insert new achievements
		QSqlQuery query(m_Database);
		query.setForwardOnly(true);
		if (!query.prepare(QString("INSERT INTO ") + TableUserAchievements::name + " (" +
			TableUserAchievements::Field::achievementID + ", " + TableUserAchievements::Field::userID + ") VALUES (?, ?)"))
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return;
		}

		query.addBindValue(_achievementID);
		query.addBindValue(_userID);
		if (!query.exec())
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return;
		}
		LOG_INFO("Session: " + m_Session.getID() + " insert user_achievement ID: " + _achievementID);
	}

	uint64_t Achievement::_collectorAchievementBelongsToGame(uint64_t _achievementID, uint64_t _gameID)
	{
		using namespace database;
		// check if achievement exists and belongs to the game
		QSqlQuery query(m_Database);
		query.setForwardOnly(true);
		if (!query.prepare(QString("SELECT ") + TableCollectorAchievements::Field::number +" FROM " + TableCollectorAchievements::name + " WHERE " +
			TableCollectorAchievements::Field::id + "= ? AND " + TableCollectorAchievements::Field::gameID + "= ?"))
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return 0;
		}
		query.addBindValue(_achievementID);
		query.addBindValue(_gameID);
		if (!query.exec())
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}

		if (!query.first())
			return 0;
		return query.value(0).toULongLong();
	}

	bool Achievement::_updateCollectorAchievement(uint64_t _achievementID, uint64_t _userID, uint64_t _number)
	{
		using namespace database;
		// check if user has achievement currently
		QSqlQuery query(m_Database);
		query.setForwardOnly(true);
		if (!query.prepare(QString("SELECT * FROM ") + TableUserCollectorAchievements::name + " WHERE " +
			TableUserCollectorAchievements::Field::achievementID + "= ? AND " + TableUserCollectorAchievements::Field::userID + "= ?"))
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}

		query.addBindValue(_achievementID);
		query.addBindValue(_userID);
		if (!query.exec())
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}

		// update
		if (query.first())
		{
			auto newCounter = query.value(TableUserCollectorAchievements::Field::counter).toULongLong() + 1;
			auto id = query.value(TableUserCollectorAchievements::Field::id);
			if (!query.prepare(QString("UPDATE ") + TableUserCollectorAchievements::name + " SET " + TableUserCollectorAchievements::Field::counter + "= ? WHERE " +
				TableUserCollectorAchievements::Field::id + "= ?"))
			{
				LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
				return false;
			}

			query.addBindValue(newCounter);
			query.addBindValue(id);
			if (!query.exec())
			{
				LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
				return false;
			}

			LOG_INFO("Session: " + m_Session.getID() + " update user_collector_achievement ID: " + _achievementID + " Counter: " + newCounter);
			return newCounter == _number;
		}

		// insert
		if (!query.prepare(QString("INSERT INTO ") + TableUserCollectorAchievements::name + " (" +
			TableUserCollectorAchievements::Field::achievementID + ", " + TableUserCollectorAchievements::Field::userID + ", " +
			TableUserCollectorAchievements::Field::counter + ") VALUES (?, ?, ?)"))
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}

		query.addBindValue(_achievementID);
		query.addBindValue(_userID);
		query.addBindValue(1);
		if (!query.exec())
		{
			LOG_ERR("Session: " + m_Session.getID() + " MySQL query error: " + query.lastError().text());
			return false;
		}

		LOG_INFO("Session: " + m_Session.getID() + " insert user_collector_achievement ID: " + _achievementID);
		return _number == 1;
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
		switch (msg.type())
		{
		case protobuf::Achievement_Type_complete:
			if (!_achievementBelongsToGame(achievementID, gameID))
				return reply;
			if (_hasAchievement(achievementID, userID))
			{
				reply.set_result(protobuf::AchievementReply_Type_exists);
				return reply;
			}
			_insertAchievement(achievementID, userID);
			reply.set_result(protobuf::AchievementReply_Type_complete);
			break;

		case protobuf::Achievement_Type_add:
		{
			auto number = _collectorAchievementBelongsToGame(achievementID, gameID);
			if (number == 0)
				return reply;
			if (_updateCollectorAchievement(achievementID, userID, number))
				reply.set_result(protobuf::AchievementReply_Type_complete);
			else
				reply.set_result(protobuf::AchievementReply_Type_progress);
			break;
		}
		default:
			LOG_ERR("Session: " + m_Session.getID() + " unhandled achievement type received: " + msg.type());
		}
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
