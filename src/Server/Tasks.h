#pragma once

#include "Database.hpp"

class Session;

namespace task
{
	std::unique_ptr<QRunnable> createTask(QByteArray _buffer, Session& _session, const database::Database& _database, uint32_t _type);

	class Base : public QRunnable, boost::noncopyable
	{
		using super = QRunnable;

	private:
		database::Database m_Database;
		Session& m_Session;
		QByteArray m_Buffer;

	public:
		Base(QByteArray _buffer, Session& _session, const database::Database& _database);
		virtual ~Base() = default;

		Session& getSession() const;
		database::Database& getDatabase();
		QByteArray getBuffer() const;
	};

	class Authenticate : public Base
	{
		using super = Base;
	public:
		Authenticate(QByteArray _buffer, Session& _session, const database::Database& _database);

		void run() override;
	};

	class Login : public Base
	{
		using super = Base;

	public:
		Login(QByteArray _buffer, Session& _session, const database::Database& _database);

		void run() override;
	};

	class Achievement : public Base
	{
		using super = Base;

	public:
		Achievement(QByteArray _buffer, Session& _session, const database::Database& _database);

		void run() override;
	};
} // namespace task
