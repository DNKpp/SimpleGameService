// stdafx.h : Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include <stdio.h>
#include <tchar.h>

// std
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <deque>
#include <cassert>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <unordered_set>
#include <unordered_map>

// boost
#include <boost/noncopyable.hpp>

// Qt
#include <QtCore/QCoreApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtCore/QCryptographicHash>

// SimpleLib
#include <SimpleLib/log/Log.h>
#include <SimpleLib/log/ConsoleOutput.h>

// others
#include "log.hpp"
