/*
SQLyog Community Edition- MySQL GUI v7.02 
MySQL - 5.0.51a-24+lenny2 : Database - gameserver
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE DATABASE /*!32312 IF NOT EXISTS*/`gameserver` /*!40100 DEFAULT CHARACTER SET latin1 COLLATE latin1_general_ci */;

USE `gameserver`;

/*Table structure for table `npc_spawns` */

DROP TABLE IF EXISTS `npc_spawns`;

CREATE TABLE `npc_spawns` (
  `npc_id` int(10) unsigned NOT NULL auto_increment,
  `npc_worldid` int(10) unsigned NOT NULL default '0',
  `npc_nameid` int(10) unsigned NOT NULL default '0',
  `npc_typeid` int(10) unsigned NOT NULL default '0',
  `npc_name` varchar(45) NOT NULL default '',
  `npc_location` int(10) unsigned NOT NULL default '0',
  `npc_x` int(10) unsigned NOT NULL default '0',
  `npc_y` int(10) unsigned NOT NULL default '0',
  `npc_z` int(10) unsigned NOT NULL default '0',
  `npc_angle` int(11) NOT NULL default '0',
  `npc_clothing` int(10) unsigned NOT NULL default '0',
  `npc_loot` int(10) unsigned NOT NULL default '0',
  `npc_unknown` int(10) unsigned NOT NULL default '0',
  `npc_trader` int(10) unsigned NOT NULL default '0',
  `npc_customname` varchar(80) default NULL,
  `npc_customscript` varchar(100) default NULL,
  `npc_shop_quality` int(3) default '50' COMMENT 'The quality of items if this npc has items to sell',
  `npc_scripting` int(1) default '1' COMMENT '1: Scripts enabled 0: Scripts disabled',
  PRIMARY KEY  (`npc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9942 DEFAULT CHARSET=latin1;

/*Data for the table `npc_spawns` */

insert  into `npc_spawns`(`npc_id`,`npc_worldid`,`npc_nameid`,`npc_typeid`,`npc_name`,`npc_location`,`npc_x`,`npc_y`,`npc_z`,`npc_angle`,`npc_clothing`,`npc_loot`,`npc_unknown`,`npc_trader`,`npc_customname`,`npc_customscript`,`npc_shop_quality`,`npc_scripting`) values (9916,1020,385,61093,'WSK',1,33808,31332,32512,30,6612,0,8814,0,'Zippy the Trader','scripts/lua/zippy.lua',50,1),(9917,1018,387,32772,'WSK',101,31938,32165,32512,130,6616,0,16675,0,'Event Info',NULL,50,1),(9918,1019,215,50585,'WSK',101,34209,33864,32512,359,197,0,19499,195,'VotR Paperboy',NULL,50,1),(9919,1020,385,13542,'WSK',101,33261,33113,32672,2,4342,0,17892,0,NULL,NULL,50,1),(9920,976,2818,15528,'WSK',2071,23436,21380,35220,185,7214,0,9785,0,NULL,NULL,50,1),(9921,971,386,5040,'WSK',2181,38000,24127,33854,264,6622,0,2590,0,NULL,NULL,50,1),(9922,1020,836,43282,'MBOTFLY',2181,43068,26020,34020,0,31788,0,405,0,NULL,NULL,50,1),(9923,988,836,41913,'MBOTFLY',2181,41537,28260,33860,0,50146,0,380,0,NULL,NULL,50,1),(9924,1006,800,15709,'MBOT',2181,42047,28256,33786,0,14348,0,220,0,NULL,NULL,50,1),(9925,1005,800,28598,'MBOT',2181,44419,23412,33445,0,8332,0,585,0,NULL,NULL,50,1),(9926,1014,672,28153,'DOGBOT',2181,38041,29414,34088,0,21948,0,221,0,NULL,NULL,50,1),(9927,984,677,32746,'WBADGUY',2181,37985,31043,33939,0,21616,0,222,0,NULL,NULL,50,1),(9928,973,606,26878,'MUTANTB',2181,26220,27158,34106,0,20679,0,490,0,NULL,NULL,50,1),(9929,975,800,11783,'MBOT',2181,31285,23969,33970,0,722,0,38,0,NULL,NULL,50,1),(9930,981,640,61001,'FLYMUT',2181,26672,26657,34281,0,20684,0,1510,0,NULL,NULL,50,1),(9931,985,610,1100,'MGMUT',2181,28040,26301,34083,0,21559,0,628,0,NULL,NULL,50,1),(9932,986,640,1159,'FLYMUT',2181,27424,27264,34328,0,21560,0,1503,0,NULL,NULL,50,1),(9933,987,626,53397,'MUTANTB',2181,26386,27672,34135,0,20680,0,407,0,NULL,NULL,50,1),(9934,999,602,19865,'MUTANTB',2181,26771,26119,34228,0,21559,0,271,0,NULL,NULL,50,1),(9935,1003,730,39449,'BROBOT',2181,25425,32173,34055,0,20939,0,4285,0,NULL,NULL,50,1);

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
