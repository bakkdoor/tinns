-- Database: `gameserver`
--
-- New stuff for NPC Subsystem
--
USE `gameserver`;

-- --------------------------------------------------------

--
-- Table structure for table `npc_spawns`
--

DROP TABLE IF EXISTS `npc_spawns`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `npc_spawns` (
	`npc_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
	`npc_worldid` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_nameid` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_typeid` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_name` varchar(45) NOT NULL DEFAULT '',
	`npc_location` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_x` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_y` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_z` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_angle` int(11) NOT NULL DEFAULT '0',
	`npc_clothing` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_loot` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_unknown` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_trader` int(10) unsigned NOT NULL DEFAULT '0',
	`npc_customname` varchar(80) DEFAULT NULL,
	`npc_customscript` varchar(100) DEFAULT NULL,
	`npc_shop_quality` int(3) DEFAULT '50' NULL COMMENT 'The quality of items if this npc has items to sell',
	PRIMARY KEY (`npc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9916 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `npc_spawns`
--

LOCK TABLES `npc_spawns` WRITE;
/*!40000 ALTER TABLE `npc_spawns` DISABLE KEYS */;
INSERT INTO `npc_spawns` (npc_worldid, npc_nameid, npc_typeid, npc_name, npc_location, npc_x, npc_y, npc_z, npc_angle, npc_clothing, npc_unknown, npc_trader, npc_customname)
VALUES 
(1020,385,61093,'WSK',1,33808,31332,32512,30,6612,8814,0,'Event Info'),
(1020,388,17634,'WSK',3,30063,34111,33136,90,6616,14035,0,'Event Info'),
(1018,387,32772,'WSK',101,31938,32165,32512,130,6616,16675,0,'Event Info'),
(1019,215,50585,'WSK',101,34209,33864,32512,359,197,19499,195,NULL),
(1020,385,13542,'WSK',101,33261,33113,32672,2,4342,17892,0,'VotR Paperboy'),
(1019,389,10890,'WSK',401,31105,32463,31936,92,6613,14572,0,'Event Info'),
(1020,1629,26858,'WSK',401,31265,30718,31952,177,2827,12567,0,NULL),
(976,2818,15528,'WSK',2071,23436,21380,35220,185,7214,9785,0,NULL),
(971,386,5040,'WSK',2181,38000,24127,33854,264,6622,2590,0,NULL),
(1020,836,43282,'MBOTFLY',2181,43068,26020,34020,0,31788,405,0,NULL),
(988,836,41913,'MBOTFLY',2181,41537,28260,33860,0,50146,380,0,NULL),
(1006,800,15709,'MBOT',2181,42047,28256,33786,0,14348,220,0,NULL),
(1005,800,28598,'MBOT',2181,44419,23412,33445,0,8332,585,0,NULL),
(1014,672,28153,'DOGBOT',2181,38041,29414,34088,0,21948,221,0,NULL),
(984,677,32746,'WBADGUY',2181,37985,31043,33939,0,21616,222,0,NULL),
(973,606,26878,'MUTANTB',2181,26220,27158,34106,0,20679,490,0,NULL),
(975,800,11783,'MBOT',2181,31285,23969,33970,0,722,38,0,NULL),
(981,640,61001,'FLYMUT',2181,26672,26657,34281,0,20684,1510,0,NULL),
(985,610,1100,'MGMUT',2181,28040,26301,34083,0,21559,628,0,NULL),
(986,640,1159,'FLYMUT',2181,27424,27264,34328,0,21560,1503,0,NULL),
(987,626,53397,'MUTANTB',2181,26386,27672,34135,0,20680,407,0,NULL),
(999,602,19865,'MUTANTB',2181,26771,26119,34228,0,21559,271,0,NULL),
(1003,730,39449,'BROBOT',2181,25425,32173,34055,0,20939,4285,0,NULL);
/*!40000 ALTER TABLE `npc_spawns` 
ENABLE KEYS */;
UNLOCK TABLES;

DROP TABLE IF EXISTS `npc_shop`;
CREATE TABLE `npc_shop`(
	`c_npc_id` int COMMENT 'The NPC WorldID',
	`c_zoneid` int COMMENT 'The ZoneID in which the NPC is',
	`c_itemid` int COMMENT 'ItemID for sale',
	`c_itemprice` int COMMENT 'Price for this item');
