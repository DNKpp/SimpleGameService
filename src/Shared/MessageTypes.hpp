#pragma once

// Some notes for future...
// >>>>>>>>>> DO NOT modify the values of this types. Only add here if we need more or others. <<<<<<<<<<<<<
namespace network 
{
namespace svr
{
	enum Type
	{
		authentication	= 1,
		login			= 2,
		achievement		= 3,
	};
} // namespace svr

namespace client
{
	enum Type
	{
		authenticationReply = 1,
		loginReply			= 2,
		achievementReply	= 3,
	};
} // namespace client
} // namespace network
