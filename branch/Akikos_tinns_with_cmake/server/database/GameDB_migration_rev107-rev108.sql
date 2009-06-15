
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
  PRIMARY KEY (`npc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9916 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `npc_spawns`
--

LOCK TABLES `npc_spawns` WRITE;
/*!40000 ALTER TABLE `npc_spawns` DISABLE KEYS */;
INSERT INTO `npc_spawns` VALUES 
(1,256,270,33900,'WSK',1,31972,31058,32552,90,33,0,19646,50,NULL,NULL),
(2,262,282,51243,'WSK',1,32811,31586,32520,180,33,0,19386,57,NULL,NULL),
(3,264,23,65186,'WLDKL',1,31573,29917,32512,180,100,0,32732,0,NULL,NULL),
(4,265,263,10711,'WSK',1,31661,30953,32544,180,101,0,19644,50,NULL,NULL),
(5,266,23,64049,'WLDK',1,29598,29969,32536,180,100,0,32705,33023,NULL,NULL),
(6,304,1,13405,'PATROL_COPBOTC',1,30059,30466,32640,180,7641,0,32299,0,NULL,NULL),
(7,306,1,56866,'PATROL_COPBOT6',1,33653,30715,32544,0,32417,0,32403,0,NULL,NULL),
(8,312,1,7303,'PATROL_COPBOT6',1,32723,32490,32648,0,791,0,32277,0,NULL,NULL),
(9,284,1622,63446,'WSK',3,29960,33508,33152,270,2652,0,29756,0,NULL,NULL),
(10,290,39,44604,'SEMPL',3,27621,33353,33152,0,92,0,30496,80,NULL,NULL),
(11,291,36,53646,'SEMPL',3,27548,33173,33152,0,92,0,30496,31,NULL,NULL),
(12,293,39,63006,'SEMPL',3,27454,33429,33152,0,92,0,30496,80,NULL,NULL),
(13,294,36,34060,'SEMPL',3,27397,33244,33152,0,92,0,30496,31,NULL,NULL),
(14,273,12,14626,'DANCER',6,32958,32949,32576,0,2538,0,4512,0,NULL,NULL),
(15,290,15,51184,'DANCER',6,33017,34974,32648,0,2537,0,572,0,NULL,NULL),
(16,281,17,52618,'DANCER',6,32975,34960,32648,0,2537,0,572,0,NULL,NULL),
(17,298,351,57844,'HOOKER',6,32958,32877,33312,90,2493,0,2208,0,NULL,NULL),
(18,264,305,3099,'WSK',9,33351,32800,32256,180,4997,0,2551,230,NULL,NULL),
(19,265,306,587,'WSK',9,33106,31095,32464,90,35249,0,363,210,NULL,NULL),
(20,263,310,57458,'WSK',24,33895,34012,32704,180,1598,0,1351,53,NULL,NULL),
(21,277,258,58731,'WSK',101,32812,32206,32512,270,94,0,22125,22,NULL,NULL),
(22,261,245,52642,'WSK',1084,33622,32546,32832,0,2967,0,5292,0,NULL,NULL),
(23,264,247,47120,'WSK',1084,32468,32928,32840,0,2651,0,5633,0,NULL,NULL),
(24,259,191,51073,'WLDK',2181,39113,24946,33921,0,10897,0,32121,0,NULL,NULL),
(25,258,191,20727,'WLDK',2181,35266,25184,33921,0,20212,0,32121,0,NULL,NULL),
(26,257,191,53363,'WLDK',2181,38849,25283,33855,0,14493,0,32121,0,NULL,NULL),
(27,256,191,48587,'WLDK',2181,38834,24848,33855,0,14493,0,32121,0,NULL,NULL),
(28,971,386,5040,'WSK',2181,38000,24127,33854,264,6622,0,2590,0,NULL,NULL),
(29,264,191,5462,'WLDK',2181,38384,26227,33855,0,1610,0,32082,0,NULL,NULL),
(30,263,1686,19955,'WSK',2181,38402,26034,33855,0,805,0,960,0,NULL,NULL),
(31,261,191,22015,'WLDK',2181,37612,25158,33855,0,59768,0,32060,0,NULL,NULL),
(32,260,191,64663,'WLDK',2181,37609,24613,33854,0,20204,0,32058,0,NULL,NULL),
(33,1020,836,43282,'MBOTFLY',2181,43068,26020,34020,0,31788,0,405,0,NULL,NULL),
(34,988,836,41913,'MBOTFLY',2181,41537,28260,33860,0,50146,0,380,0,NULL,NULL),
(35,1006,800,15709,'MBOT',2181,42047,28256,33786,0,14348,0,220,0,NULL,NULL),
(36,1005,800,28598,'MBOT',2181,44419,23412,33445,0,8332,0,585,0,NULL,NULL),
(37,1014,672,28153,'DOGBOT',2181,38041,29414,34088,0,21948,0,221,0,NULL,NULL),
(38,984,677,32746,'WBADGUY',2181,37985,31043,33939,0,21616,0,222,0,NULL,NULL),
(39,262,191,2071,'WLDK',2181,33772,27289,34104,135,58458,0,32005,0,NULL,NULL),
(40,973,606,26878,'MUTANTB',2181,26220,27158,34106,0,20679,0,490,0,NULL,NULL),
(41,975,800,11783,'MBOT',2181,31285,23969,33970,0,722,0,38,0,NULL,NULL),
(42,981,640,61001,'FLYMUT',2181,26672,26657,34281,0,20684,0,1510,0,NULL,NULL),
(43,985,610,1100,'MGMUT',2181,28040,26301,34083,0,21559,0,628,0,NULL,NULL),
(44,986,640,1159,'FLYMUT',2181,27424,27264,34328,0,21560,0,1503,0,NULL,NULL),
(45,987,626,53397,'MUTANTB',2181,26386,27672,34135,0,20680,0,407,0,NULL,NULL),
(46,999,602,19865,'MUTANTB',2181,26771,26119,34228,0,21559,0,271,0,NULL,NULL),
(47,1003,730,39449,'BROBOT',2181,25425,32173,34055,0,20939,0,4285,0,NULL,NULL);
/*!40000 ALTER TABLE `npc_spawns` 
ENABLE KEYS */;
UNLOCK TABLES;
