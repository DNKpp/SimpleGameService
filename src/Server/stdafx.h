// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once

#include "targetver.h"

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

// boost
#include <boost/noncopyable.hpp>

// Qt
#include <QtCore/QCoreApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtSql/QSqlDatabase>

// SimpleLib
#include <SimpleLib/log/Log.h>
#include <SimpleLib/log/ConsoleOutput.h>

// others
#include "log.hpp"
