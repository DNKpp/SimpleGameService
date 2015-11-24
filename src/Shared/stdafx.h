// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
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
#include <stdexcept>

// boost
#include <boost/noncopyable.hpp>

// Qt
#include <QtCore/QtCore>
#include <QtConcurrent/QtConcurrent>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// SimpleLib
#include <SimpleLib/log/Log.h>
#include <SimpleLib/log/ConsoleOutput.h>
#include <SimpleLib/log/HTMLFileOutput.h>
#include <SimpleLib/Flag.h>

// others
#include "log.hpp"
#include "SharedDefines.hpp"
