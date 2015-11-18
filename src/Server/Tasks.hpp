#pragma once

#include "Database.hpp"

class Session;

namespace protobuf
{
	class AchievementReply;
}

namespace task
{
	using Result = std::pair<QByteArray, network::MessageType>;

	class Base : boost::noncopyable
	{
	private:
		virtual Result run() = 0;

	protected:
		const uint64_t m_Index;
		QByteArray m_Buffer;
		Session& m_Session;
		QSqlDatabase m_Database;

	public:
		Base(QByteArray _buffer, Session& _session, uint64_t _index);
		~Base();

		Result start();
	};

	class Authenticate : public Base
	{
	private:
		using super = Base;

		bool _authGame();
		Result run() override;

	public:
		Authenticate(QByteArray _buffer, Session& _session, uint64_t _index);
	};

	class Login : public Base
	{
	private:
		using super = Base;

		bool _tryLogin();
		Result run() override;

	public:
		Login(QByteArray _buffer, Session& _session, uint64_t _index);
	};

	class Achievement : public Base
	{
	private:
		using super = Base;

		protobuf::AchievementReply _check();
		protobuf::AchievementReply _checkSingle();
		protobuf::AchievementReply _checkAdd();

		Result run() override;

	public:
		Achievement(QByteArray _buffer, Session& _session, uint64_t _index);
	};

	class TaskWatcher : public QObject
	{
		Q_OBJECT
	private:
		using super = QObject;

		QFuture<Result> m_Future;
		QFutureWatcher<Result> m_Watcher;
		std::unique_ptr<Base> m_Task;

		Session& m_Session;

	public:
		TaskWatcher(Session& _session, QObject* _parent);
		~TaskWatcher();

		void run(QByteArray _buffer, network::MessageType id);

	signals:
		void replyCreated(QByteArray, network::MessageType);

		private slots:
		void _onFinished();
	};
} // namespace task
