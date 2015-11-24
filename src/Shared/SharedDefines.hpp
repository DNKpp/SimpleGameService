#pragma once

namespace network
{
	static const std::string PacketBegin{ "simpledata" };

	using VersionType = uint16_t;
	using MessageType = uint32_t;
	using MessageSizeType = uint32_t;

	// Some notes for future...
	// >>>>>>>>>> DO NOT modify the values of this types. Only add here if we need more or others. <<<<<<<<<<<<<
namespace svr
{
	enum Type
	{
		authentication	= 1,
		login			= 2,
		achievement		= 3,
		welcome			= 4,
		aes_key			= 5,
	};
} // namespace svr

namespace client
{
	enum Type
	{
		authenticationReply	= 1,
		loginReply			= 2,
		achievementReply	= 3,
		welcome				= 4,
		cryptReply			= 5,
	};
} // namespace client
} // namespace network
