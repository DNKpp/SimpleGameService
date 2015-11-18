#include "stdafx.h"
#include "DatabaseTables.hpp"

namespace database
{
	// TableGames
	const char* TableGames::name = "Games";
	const char* TableGames::Field::id = "ID";
	const char* TableGames::Field::hash = "Hash";
	const char* TableGames::Field::ownerID = "OwnerID";

	// TableUsers
	const char* TableUsers::name = "Users";
	const char* TableUsers::Field::id = "ID";
	const char* TableUsers::Field::name = "Name";
	const char* TableUsers::Field::password = "Password";
	const char* TableUsers::Field::salt = "Salt";

	// TableAchievements
	const char* TableAchievements::name = "Achievements";
	const char* TableAchievements::Field::id = "ID";
	const char* TableAchievements::Field::gameID = "GameID";
	const char* TableAchievements::Field::name = "Name";

	// TableUserAchievements
	const char* TableUserAchievements::name = "User_Achievements";
	const char* TableUserAchievements::Field::id = "ID";
	const char* TableUserAchievements::Field::userID = "UserID";
	const char* TableUserAchievements::Field::achievementID = "achievementID";

	// TableCollectorAchievements
	const char* TableCollectorAchievements::name = "collector_achievements";
	const char* TableCollectorAchievements::Field::id = "ID";
	const char* TableCollectorAchievements::Field::gameID = "GameID";
	const char* TableCollectorAchievements::Field::number = "Number";
	const char* TableCollectorAchievements::Field::name = "Name";

	// TableUserCollectorAchievements
	const char* TableUserCollectorAchievements::name = "user_collector_achievements";
	const char* TableUserCollectorAchievements::Field::id = "ID";
	const char* TableUserCollectorAchievements::Field::userID = "UserID";
	const char* TableUserCollectorAchievements::Field::achievementID = "achievementID";
	const char* TableUserCollectorAchievements::Field::counter = "Counter";
} // namespace database
