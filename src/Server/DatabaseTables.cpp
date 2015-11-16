#include "stdafx.h"
#include "DatabaseTables.h"

namespace database
{
	// TableGames
	const char* TableGames::name = "Games";
	const char* TableGames::Field::id = "ID";
	const char* TableGames::Field::hash = "Hash";
	const char* TableGames::Field::ownerID = "OwnerID";


} // namespace database
