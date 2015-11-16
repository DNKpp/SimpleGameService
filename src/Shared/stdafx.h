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

// boost
#include <boost/noncopyable.hpp>

// Qt
#include <QtCore/QCoreApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

// SimpleLib
#include <SimpleLib/log/Log.h>
#include <SimpleLib/log/ConsoleOutput.h>

// others
#include "log.hpp"
