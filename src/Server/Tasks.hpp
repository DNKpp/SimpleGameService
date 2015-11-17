#pragma once

#include "Database.hpp"

class Session;

namespace task
{
	using Result = std::pair<QByteArray, uint32_t>;

	class TaskWatcher : public QObject
	{
		Q_OBJECT
	private:
		using super = QObject;

		QFuture<Result> m_Future;
		QFutureWatcher<Result> m_Watcher;

		Session& m_Session;

	public:
		TaskWatcher(Session& _session, QObject* _parent);
		~TaskWatcher();

		void run(QByteArray _buffer, uint32_t id);

	signals:
		void replyCreated(QByteArray, uint32_t);

	private slots:
		void _onFinished();
	};

	class Authenticate
	{
	private:
		QByteArray m_Buffer;
		Session& m_Session;

		bool _authGame();

	public:
		Authenticate(QByteArray _buffer, Session& _session);

		Result run();
	};

	class Login
	{
	private:
		QByteArray m_Buffer;
		Session& m_Session;

		bool _tryLogin();

	public:
		Login(QByteArray _buffer, Session& _session);

		Result run();
	};

	//class Achievement
	//{
	//public:
	//	Result run(QByteArray _buffer, Session& _session, database::Database& _database);
	//};
} // namespace task
