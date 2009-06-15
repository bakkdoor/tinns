-- phpMyAdmin SQL Dump
-- version 2.8.1
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: May 28, 2007 at 10:50 PM
-- Server version: 4.1.14
-- PHP Version: 5.2.0
-- 
-- Database: `gameserver`
-- 
CREATE DATABASE `gameserver` DEFAULT CHARACTER SET latin1 COLLATE latin1_general_ci;
USE `gameserver`;

-- --------------------------------------------------------

-- 
-- Table structure for table `apartments`
-- 

CREATE TABLE `apartments` (
  `apt_id` int(10) unsigned NOT NULL auto_increment,
  `apt_location` int(10) unsigned NOT NULL default '0',
  `apt_type` int(10) unsigned NOT NULL default '0',
  `apt_password` varchar(45) NOT NULL default '',
  `apt_owner` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`apt_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `apt_doors`
-- 

CREATE TABLE `apt_doors` (
  `ad_id` int(10) unsigned NOT NULL auto_increment,
  `ad_apt_id` int(10) unsigned NOT NULL default '0',
  `ad_type` int(10) unsigned NOT NULL default '0',
  `ad_apt_map` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ad_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `apt_items`
-- 

CREATE TABLE `apt_items` (
  `ai_id` int(10) unsigned NOT NULL auto_increment,
  `ai_apt_id` int(10) unsigned NOT NULL default '0',
  `ai_apt_map` int(10) unsigned NOT NULL default '0',
  `ai_type` int(10) unsigned NOT NULL default '0',
  `ai_option1` int(10) unsigned NOT NULL default '0',
  `ai_option2` int(10) unsigned NOT NULL default '0',
  `ai_option3` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ai_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `buddy_list`
-- 

CREATE TABLE `buddy_list` (
  `bud_id` int(10) NOT NULL auto_increment,
  `bud_charid` int(10) NOT NULL default '0',
  `bud_buddyid` mediumint(10) NOT NULL default '0',
  PRIMARY KEY  (`bud_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `bug report`
-- 

CREATE TABLE `bug report` (
  `br_id` int(10) unsigned NOT NULL auto_increment,
  `br_type` int(10) unsigned NOT NULL default '0',
  `br_desc` blob NOT NULL,
  `br_fromid` int(10) unsigned NOT NULL default '0',
  `br_location` int(10) unsigned NOT NULL default '0',
  `br_status` int(10) unsigned NOT NULL default '0',
  `br_datetime` varchar(45) NOT NULL default '',
  `br_supid` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`br_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `characters`
-- 

CREATE TABLE `characters` (
  `c_id` int(10) unsigned NOT NULL auto_increment,
  `c_name` varchar(45) default '',
  `c_str_lvl` int(10) unsigned default '0',
  `c_str_pts` int(10) unsigned default '0',
  `c_int_lvl` int(10) unsigned default '0',
  `c_int_pts` int(10) unsigned default '0',
  `c_dex_lvl` int(10) unsigned default '0',
  `c_dex_pts` int(10) unsigned default '0',
  `c_con_lvl` int(10) unsigned default '0',
  `c_con_pts` int(10) unsigned default '0',
  `c_psi_lvl` int(10) unsigned default '0',
  `c_psi_pts` int(10) unsigned default '0',
  `a_id` int(10) unsigned default '0',
  `c_class` int(10) unsigned default '0',
  `c_profession` int(10) unsigned default '0',
  `c_sex` int(10) unsigned default '0',
  `c_location` int(10) unsigned default '1',
  `c_mc` int(10) unsigned default '0',
  `c_hc` int(10) unsigned default '0',
  `c_tra` int(10) unsigned default '0',
  `c_pc` int(10) unsigned default '0',
  `c_rc` int(10) unsigned default '0',
  `c_tc` int(10) unsigned default '0',
  `c_vhc` int(10) unsigned default '0',
  `c_agl` int(10) unsigned default '0',
  `c_rep` int(10) unsigned default '0',
  `c_rec` int(10) unsigned default '0',
  `c_rcl` int(10) unsigned default '0',
  `c_atl` int(10) unsigned default '0',
  `c_end` int(10) unsigned default '0',
  `c_for` int(10) unsigned default '0',
  `c_fir` int(10) unsigned default '0',
  `c_enr` int(10) unsigned default '0',
  `c_xrr` int(10) unsigned default '0',
  `c_por` int(10) unsigned default '0',
  `c_htl` int(10) unsigned default '0',
  `c_hck` int(10) unsigned default '0',
  `c_brt` int(10) unsigned default '0',
  `c_psu` int(10) unsigned default '0',
  `c_wep` int(10) unsigned default '0',
  `c_cst` int(10) unsigned default '0',
  `c_res` int(10) unsigned default '0',
  `c_imp` int(10) unsigned default '0',
  `c_ppu` int(10) unsigned default '0',
  `c_apu` int(10) unsigned default '0',
  `c_mst` int(10) unsigned default '0',
  `c_ppw` int(10) unsigned default '0',
  `c_psr` int(10) unsigned default '0',
  `c_wpw` int(10) unsigned default '0',
  `c_apt` int(10) unsigned default '0',
  `c_cash` int(10) unsigned default '0',
  `c_head` int(10) unsigned default '0',
  `c_torso` int(10) unsigned default '0',
  `c_legs` int(10) unsigned default '0',
  `c_str_xp` float default '0',
  `c_int_xp` float default '0',
  `c_dex_xp` float default '0',
  `c_psi_xp` float default '0',
  `c_con_xp` float default '0',
  `c_pos_x` float NOT NULL default '0',
  `c_pos_y` float NOT NULL default '0',
  `c_pos_z` float NOT NULL default '0',
  `c_angle_ud` float NOT NULL default '0',
  `c_angle_lr` float NOT NULL default '0',
  `c_faction` int(10) unsigned NOT NULL default '0',
  `c_slot` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`c_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `contacts`
-- 

CREATE TABLE `contacts` (
  `c_id` int(10) unsigned NOT NULL auto_increment,
  `c_listid` int(10) unsigned NOT NULL default '0',
  `c_conid` int(10) unsigned NOT NULL default '0',
  `c_type` int(10) unsigned NOT NULL default '0',
  `c_desc` blob NOT NULL,
  PRIMARY KEY  (`c_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `emails`
-- 

CREATE TABLE `emails` (
  `e_id` int(10) unsigned NOT NULL auto_increment,
  `e_subject` varchar(128) NOT NULL default '',
  `e_fromid` int(10) unsigned NOT NULL default '0',
  `e_datetime` varchar(45) NOT NULL default '0',
  `e_toid` int(10) unsigned NOT NULL default '0',
  `e_body` blob NOT NULL,
  `e_new` tinyint(1) NOT NULL default '1',
  `e_replied` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`e_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `forum_posts`
-- 

CREATE TABLE `forum_posts` (
  `fp_id` int(10) unsigned NOT NULL auto_increment,
  `fp_name` varchar(45) NOT NULL default '',
  `fp_fromid` int(10) unsigned NOT NULL default '0',
  `fp_content` blob NOT NULL,
  `fp_datetime` varchar(45) NOT NULL default '',
  `fp_replyid` int(10) unsigned NOT NULL default '0',
  `fp_forumid` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`fp_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `forums`
-- 

CREATE TABLE `forums` (
  `f_id` int(10) unsigned NOT NULL auto_increment,
  `f_name` varchar(45) NOT NULL default '',
  `f_area` int(10) unsigned NOT NULL default '0',
  `f_showname` varchar(45) NOT NULL default '',
  PRIMARY KEY  (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=21 ;

-- 
-- Dumping data for table `forums`
-- 

INSERT INTO `forums` (`f_id`, `f_name`, `f_area`, `f_showname`) VALUES (1, 'bbsnctradeweap', 0, 'nc.trading.weapons/armor'),
(2, 'bbsnctraderare', 0, 'nc.trading.rareparts'),
(3, 'bbsnctradeimp', 0, 'nc.trading.implants/boneenforcements'),
(4, 'bbsnctradepsi', 0, 'nc.trading.psi equipment'),
(5, 'bbsnctradeblue', 0, 'nc.trading.blueprints'),
(6, 'bbsnctradeitem', 0, 'nc.trading.itemparts/raw materials'),
(7, 'bbsnctradedrug', 0, 'nc.trading.drugs'),
(8, 'bbsneocronmarket', 0, 'nc.trading.miscellaneous'),
(9, 'bbsncrunnerano', 1, 'nc.runner.anouncements'),
(10, 'bbsncrunnerserv', 1, 'nc.runner.services'),
(11, 'bbsncrunnereven', 1, 'nc.runner.events'),
(12, 'bbsncrunnerauct', 1, 'nc.runner.auctions'),
(13, 'bbsncrunnerclan', 1, 'nc.runner.clan recruitment'),
(14, 'bbsncrunnerfind', 1, 'nc.runner.find team'),
(15, 'bbsncrunnerhelp', 1, 'nc.runner.runner2runnerhelp'),
(16, 'bbsneocrongeneral', 2, 'nc.talk.general'),
(17, 'bbsnctalkpolitics', 2, 'nc.talk.political discussions'),
(18, 'bbfgeneral', 3, 'private.faction.general'),
(19, 'bbcgeneral', 4, 'private.clan.general'),
(20, 'bbsneobugreport', 2, 'Neopolis Bug Report');

-- --------------------------------------------------------

-- 
-- Table structure for table `genrep`
-- 

CREATE TABLE `genrep` (
  `g_id` int(10) unsigned NOT NULL auto_increment,
  `g_worldid` int(10) unsigned NOT NULL default '0',
  `g_stationid` int(10) unsigned NOT NULL default '0',
  `g_charid` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`g_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `guides`
-- 

CREATE TABLE `guides` (
  `g_id` int(10) unsigned NOT NULL auto_increment,
  `g_type` int(10) unsigned NOT NULL default '0',
  `g_title` varchar(45) NOT NULL default '',
  `g_content` blob NOT NULL,
  PRIMARY KEY  (`g_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `inventory`
-- 

CREATE TABLE `inventory` (
  `inv_id` int(10) unsigned NOT NULL auto_increment,
  `inv_charid` int(10) unsigned NOT NULL default '0',
  `inv_loc` int(10) unsigned NOT NULL default '0',
  `inv_x` int(10) unsigned NOT NULL default '0',
  `inv_y` int(10) unsigned NOT NULL default '0',
  `inv_itemid` int(10) unsigned NOT NULL default '0',
  `inv_type` int(10) unsigned NOT NULL default '0',
  `inv_qty` int(10) unsigned NOT NULL default '0',
  `inv_curdur` int(10) unsigned NOT NULL default '0',
  `inv_dmg` int(10) unsigned NOT NULL default '0',
  `inv_freq` int(10) unsigned NOT NULL default '0',
  `inv_hand` int(10) unsigned NOT NULL default '0',
  `inv_rng` int(10) unsigned NOT NULL default '0',
  `inv_maxdur` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`inv_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `neochronicle`
-- 

CREATE TABLE `neochronicle` (
  `nc_id` int(10) unsigned NOT NULL auto_increment,
  `nc_name` varchar(45) NOT NULL default '',
  `nc_content` blob NOT NULL,
  `nc_datetime` varchar(45) NOT NULL default '',
  PRIMARY KEY  (`nc_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `neocron articles`
-- 

CREATE TABLE `neocron articles` (
  `na_id` int(10) unsigned NOT NULL auto_increment,
  `na_datetime` varchar(45) NOT NULL default '',
  `na_author` varchar(45) NOT NULL default '0',
  `na_name` varchar(45) NOT NULL default '',
  `na_content` blob NOT NULL,
  `na_approval` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`na_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `npc_spawns`
-- 

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

--
-- Dumping data for table `npc_spawns`
--

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

-- --------------------------------------------------------

-- 
-- Table structure for table `stockx`
-- 

CREATE TABLE `stockx` (
  `st_id` int(10) unsigned NOT NULL auto_increment,
  `st_factid` int(10) unsigned NOT NULL default '0',
  `st_curval` float NOT NULL default '0',
  `st_oldval` float NOT NULL default '0',
  PRIMARY KEY  (`st_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=12 ;

-- 
-- Dumping data for table `stockx`
-- 

INSERT INTO `stockx` (`st_id`, `st_factid`, `st_curval`, `st_oldval`) VALUES (1, 9, 1227, 1227),
(2, 2, 2499, 2400),
(3, 6, 1269, 1269),
(4, 10, 1387, 1387),
(5, 8, 676, 676),
(6, 14, 783, 783),
(7, 15, 591, 591),
(8, 3, 1779, 1779),
(9, 5, 879, 879),
(10, 11, 1581, 1581),
(11, 4, 1544, 1544);

-- --------------------------------------------------------

-- 
-- Table structure for table `vehicles`
-- 

CREATE TABLE `vehicles` (
  `v_id` int(10) unsigned NOT NULL auto_increment,
  `v_owner` int(10) unsigned NOT NULL default '0',
  `v_type` int(10) unsigned NOT NULL default '0',
  `v_condition` int(10) unsigned NOT NULL default '0',
  `v_status` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`v_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `world_actors`
-- 

CREATE TABLE `world_actors` (
  `wa_id` int(10) unsigned NOT NULL auto_increment COMMENT 'Unique ID',
  `wa_actor_id` int(10) unsigned default NULL COMMENT 'u32 ID for the worldactor',
  `wa_actor_map` int(10) unsigned default NULL COMMENT 'World/Zone ID',
  `wa_actor_model` int(10) unsigned default NULL COMMENT 'ID of worldactor (pak_models.ini)',
  `wa_actor_type` int(10) unsigned default NULL COMMENT 'Function type of the actor',
  `wa_posX` int(10) unsigned default NULL,
  `wa_posY` int(10) unsigned default NULL,
  `wa_posZ` int(10) unsigned default NULL,
  `wa_rotX` int(10) unsigned default NULL,
  `wa_rotY` int(10) unsigned default NULL,
  `wa_rotZ` int(10) unsigned default NULL,
  `wa_option1` int(10) unsigned default NULL,
  `wa_option2` int(10) unsigned default NULL,
  `wa_option3` int(10) unsigned default NULL,
  PRIMARY KEY  (`wa_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `world_doors`
-- 

CREATE TABLE `world_doors` (
  `wd_id` int(10) unsigned NOT NULL auto_increment,
  `wd_world_id` int(10) unsigned NOT NULL default '0',
  `wd_type` int(10) unsigned NOT NULL default '0',
  `wd_world_map` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`wd_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


-- --------------------------------------------------------

-- 
-- Table structure for table `world_items`
-- 

CREATE TABLE `world_items` (
  `wi_id` int(10) unsigned NOT NULL auto_increment,
  `wi_worlditem_id` int(10) unsigned NOT NULL default '0',
  `wi_worlditem_map` int(10) unsigned NOT NULL default '0',
  `wi_type` int(10) unsigned NOT NULL default '0',
  `wi_option1` int(10) unsigned NOT NULL default '0',
  `wi_option2` int(10) unsigned NOT NULL default '0',
  `wi_option3` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`wi_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;
