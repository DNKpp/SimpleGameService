#pragma once

const unsigned short PORT = 1500;

extern sl::log::Log GlobalLog;

#define LOG_DEBUG(msg) DEBUG_MESSAGE(GlobalLog, msg);
#define LOG_INFO(msg) INFO_MESSAGE(GlobalLog, msg);
#define LOG_WARN(msg) WARNING_MESSAGE(GlobalLog, msg);
#define LOG_ERR(msg) ERROR_MESSAGE(GlobalLog, msg);
#define LOG_CRITIC(msg) CRITICAL_MESSAGE(GlobalLog, msg);
