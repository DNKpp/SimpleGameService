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

	struct TableUsers
	{
		static const char* name;
		struct Field
		{
			static const char* id;
			static const char* name;
			static const char* password;
			static const char* salt;
		};
	};

	struct TableAchievements
	{
		static const char* name;
		struct Field
		{
			static const char* id;
			static const char* gameID;
			static const char* name;
		};
	};

	struct TableUserAchievements
	{
		static const char* name;
		struct Field
		{
			static const char* id;
			static const char* userID;
			static const char* achievementID;
		};
	};
} // namespace database
