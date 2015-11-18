CREATE TABLE `users` (
 `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
 `Name` varchar(20) NOT NULL,
 `Password` binary(32) NOT NULL,
 `Salt` binary(32) NOT NULL,
 PRIMARY KEY (`ID`),
 UNIQUE KEY `Name` (`Name`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8

CREATE TABLE `games` (
 `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
 `Hash` binary(32) NOT NULL,
 `OwnerID` int(11) NOT NULL,
 PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8

CREATE TABLE `achievement_infos` (
 `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
 `Name` varchar(50) NOT NULL,
 PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8

CREATE TABLE `achievements` (
 `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
 `GameID` int(10) unsigned NOT NULL,
 `InfoID` int(11) NOT NULL,
 PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8

CREATE TABLE `collector_achievements` (
 `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
 `GameID` int(10) unsigned NOT NULL,
 `Number` int(10) unsigned NOT NULL,
 `InfoID` int(10) unsigned NOT NULL,
 PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8

CREATE TABLE `user_achievements` (
 `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
 `achievementID` int(10) unsigned NOT NULL,
 `userID` int(10) unsigned NOT NULL,
 PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8

CREATE TABLE `user_collector_achievements` (
 `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
 `achievementID` int(10) unsigned NOT NULL,
 `UserID` int(10) unsigned NOT NULL,
 `counter` int(10) unsigned NOT NULL,
 PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8