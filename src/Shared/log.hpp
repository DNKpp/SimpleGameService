#pragma once

extern sl::log::Log GlobalLog;

template <sl::log::MessageType Type>
sl::log::Stream<Type>& operator +(sl::log::Stream<Type>& _stream, const QString& _str)
{
	return _stream + _str.toStdString();
}

#define LOG_DEBUG(msg) DEBUG_MESSAGE(GlobalLog, msg);
#define LOG_INFO(msg) INFO_MESSAGE(GlobalLog, msg);
#define LOG_WARN(msg) WARNING_MESSAGE(GlobalLog, msg);
#define LOG_ERR(msg) ERROR_MESSAGE(GlobalLog, msg);
#define LOG_CRITIC(msg) CRITICAL_MESSAGE(GlobalLog, msg);
