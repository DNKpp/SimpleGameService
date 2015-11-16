#pragma once

namespace database
{
	struct TableGames
	{
		static const char* name;
		struct Field
		{
			static const char* id;
			static const char* hash;
			static const char* ownerID;
		};
	};
} // namespace database
