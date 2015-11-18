#pragma once

namespace network
{
	static const std::string PacketBegin{ "simpledata" };

	using VersionType = uint16_t;
	using MessageType = uint32_t;
	using MessageSizeType = uint32_t;
} // namespace network
