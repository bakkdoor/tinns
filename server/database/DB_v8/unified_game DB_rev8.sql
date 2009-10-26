/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE DATABASE /*!32312 IF NOT EXISTS*/`gameserver` /*!40100 DEFAULT CHARACTER SET latin1 COLLATE latin1_general_ci */;

USE `gameserver`;

/*Table structure for table `apartments` */

DROP TABLE IF EXISTS `apartments`;

CREATE TABLE `apartments` (
  `apt_id` int(10) unsigned NOT NULL auto_increment,
  `apt_location` int(10) unsigned NOT NULL default '0',
  `apt_type` int(10) unsigned NOT NULL default '0',
  `apt_password` varchar(45) NOT NULL default '',
  `apt_owner` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`apt_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

/*Table structure for table `buddy_list` */

DROP TABLE IF EXISTS `buddy_list`;

CREATE TABLE `buddy_list` (
  `bud_id` int(10) NOT NULL auto_increment,
  `bud_charid` int(10) NOT NULL default '0',
  `bud_buddyid` mediumint(10) NOT NULL default '0',
  PRIMARY KEY  (`bud_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Table structure for table `bug_report` */

DROP TABLE IF EXISTS `bug_report`;

CREATE TABLE `bug_report` (
  `br_id` int(10) unsigned NOT NULL auto_increment,
  `br_type` int(10) unsigned NOT NULL default '0' COMMENT 'Type',
  `br_desc` varchar(1024) NOT NULL COMMENT 'Description',
  `br_fromid` int(10) unsigned NOT NULL default '0' COMMENT 'Char ID of reporter',
  `br_location` int(10) unsigned NOT NULL default '0' COMMENT 'Location where problem occured',
  `br_status` int(10) unsigned NOT NULL default '0' COMMENT 'Status',
  `br_datetime` varchar(45) NOT NULL COMMENT 'Date/Time',
  `br_supid` int(10) unsigned NOT NULL default '0' COMMENT 'Supporter ID',
  PRIMARY KEY  (`br_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Table structure for table `characters` */

DROP TABLE IF EXISTS `characters`;

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
  `c_online` int(1) default '0',
  `c_clan` int(10) default '0',
  `c_soullight` int(3) default '10',
  PRIMARY KEY  (`c_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

/*Table structure for table `clanlevels` */

DROP TABLE IF EXISTS `clanlevels`;

CREATE TABLE `clanlevels` (
  `cll_id` int(10) NOT NULL auto_increment,
  `cll_clanid` int(10) default '0',
  `cll_level` int(2) default '0',
  `cll_desc` char(255) collate latin1_general_ci default NULL,
  `cll_charid` int(10) default NULL,
  PRIMARY KEY  (`cll_id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

/*Table structure for table `clans` */

DROP TABLE IF EXISTS `clans`;

CREATE TABLE `clans` (
  `cl_id` int(10) unsigned NOT NULL auto_increment,
  `cl_shortdesc` char(16) collate latin1_general_ci default NULL,
  `cl_name` char(64) collate latin1_general_ci default NULL,
  `cl_faction` int(2) default NULL,
  `cl_description` varchar(256) collate latin1_general_ci default NULL,
  `cl_leader` int(10) default '0',
  `cl_money` int(10) default '0',
  `cl_minsympathy` int(3) default '70',
  `cl_appid` int(10) default '0',
  `cl_representative` int(10) default '0',
  PRIMARY KEY  (`cl_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1002 DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

/*Table structure for table `clanwars` */

DROP TABLE IF EXISTS `clanwars`;

CREATE TABLE `clanwars` (
  `cw_id` int(10) NOT NULL auto_increment,
  `cw_initclan` int(10) default '0',
  `cw_enemyclan` int(10) default '0',
  `cw_starttime` varchar(45) collate latin1_general_ci default '',
  `cw_status` int(2) default '0',
  `cw_statement_initiator` varchar(512) collate latin1_general_ci default '',
  `cw_statement_enemy` varchar(512) collate latin1_general_ci default '',
  PRIMARY KEY  (`cw_id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

/*Table structure for table `contacts` */

DROP TABLE IF EXISTS `contacts`;

CREATE TABLE `contacts` (
  `c_id` int(10) unsigned NOT NULL auto_increment,
  `c_listid` int(10) unsigned NOT NULL default '0' COMMENT 'Who''s list?',
  `c_conid` int(10) unsigned NOT NULL default '0' COMMENT 'Char ID of person on list',
  `c_type` int(10) unsigned NOT NULL default '0' COMMENT '1=Personal, 2=Business, 3=Allied',
  `c_desc` varchar(256) NOT NULL,
  PRIMARY KEY  (`c_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

/*Table structure for table `datacubes` */

DROP TABLE IF EXISTS `datacubes`;

CREATE TABLE `datacubes` (
  `cb_id` int(10) NOT NULL auto_increment,
  `cb_securitycode` char(30) collate latin1_general_ci default '',
  `cb_inscription` tinytext collate latin1_general_ci,
  PRIMARY KEY  (`cb_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

/*Table structure for table `emails` */

DROP TABLE IF EXISTS `emails`;

CREATE TABLE `emails` (
  `e_id` int(10) unsigned NOT NULL auto_increment,
  `e_subject` varchar(128) NOT NULL default '',
  `e_fromid` int(10) unsigned NOT NULL default '0',
  `e_datetime` varchar(45) NOT NULL default '0',
  `e_toid` int(10) unsigned NOT NULL default '0',
  `e_body` varchar(512) NOT NULL,
  `e_new` tinyint(1) NOT NULL default '1',
  `e_replied` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`e_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

/*Table structure for table `forum_posts` */

DROP TABLE IF EXISTS `forum_posts`;

CREATE TABLE `forum_posts` (
  `fp_id` int(10) unsigned NOT NULL auto_increment,
  `fp_name` varchar(45) NOT NULL default '',
  `fp_fromid` int(10) unsigned NOT NULL default '0',
  `fp_content` varchar(2048) NOT NULL default '',
  `fp_datetime` varchar(45) NOT NULL default '',
  `fp_replyid` int(10) unsigned NOT NULL default '0',
  `fp_forumid` int(10) unsigned NOT NULL default '0',
  `fp_factionid` int(2) default '0',
  `fp_clanid` int(10) default '0',
  PRIMARY KEY  (`fp_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

/*Table structure for table `forums` */

DROP TABLE IF EXISTS `forums`;

CREATE TABLE `forums` (
  `f_id` int(10) unsigned NOT NULL auto_increment,
  `f_name` varchar(45) NOT NULL default '',
  `f_area` int(10) unsigned NOT NULL default '0',
  `f_showname` varchar(45) NOT NULL default '',
  PRIMARY KEY  (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=latin1;

/*Table structure for table `genrep` */

DROP TABLE IF EXISTS `genrep`;

CREATE TABLE `genrep` (
  `g_id` int(10) unsigned NOT NULL auto_increment,
  `g_worldid` int(10) unsigned NOT NULL default '0',
  `g_stationid` int(10) unsigned NOT NULL default '0',
  `g_charid` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`g_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Table structure for table `guides` */

DROP TABLE IF EXISTS `guides`;

CREATE TABLE `guides` (
  `g_id` int(10) unsigned NOT NULL auto_increment,
  `g_type` int(10) unsigned NOT NULL default '0',
  `g_title` varchar(45) NOT NULL default '',
  `g_language` int(2) default NULL,
  `g_content` text NOT NULL,
  `g_chapter` int(1) default '0',
  `g_part` int(1) default '0',
  PRIMARY KEY  (`g_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

/*Table structure for table `inventory` */

DROP TABLE IF EXISTS `inventory`;

CREATE TABLE `inventory` (
  `inv_id` int(10) unsigned NOT NULL auto_increment,
  `inv_charid` int(10) unsigned NOT NULL default '0',
  `inv_loc` int(10) unsigned NOT NULL default '0',
  `inv_x` int(10) unsigned NOT NULL default '0',
  `inv_y` int(10) unsigned NOT NULL default '0',
  `inv_itemid` int(10) unsigned NOT NULL default '0',
  `inv_flag` int(10) unsigned NOT NULL default '0',
  `inv_qty` int(10) unsigned NOT NULL default '0',
  `inv_sqty` int(10) unsigned NOT NULL default '0',
  `inv_cdur` int(10) unsigned NOT NULL default '0',
  `inv_dmg` int(10) unsigned NOT NULL default '0',
  `inv_frq` int(10) unsigned NOT NULL default '0',
  `inv_hnd` int(10) unsigned NOT NULL default '0',
  `inv_rng` int(10) unsigned NOT NULL default '0',
  `inv_mdur` int(10) unsigned NOT NULL default '0',
  `inv_slots` int(10) unsigned NOT NULL default '0',
  `inv_slt1` int(10) unsigned NOT NULL default '0',
  `inv_slt2` int(10) unsigned NOT NULL default '0',
  `inv_slt3` int(10) unsigned NOT NULL default '0',
  `inv_slt4` int(10) unsigned NOT NULL default '0',
  `inv_slt5` int(10) unsigned NOT NULL default '0',
  `inv_atype` int(10) unsigned NOT NULL default '0',
  `inv_contain` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`inv_id`)
) ENGINE=InnoDB AUTO_INCREMENT=19 DEFAULT CHARSET=latin1;

/*Table structure for table `moneytransactions` */

DROP TABLE IF EXISTS `moneytransactions`;

CREATE TABLE `moneytransactions` (
  `mt_id` int(10) NOT NULL auto_increment,
  `mt_clanid` int(10) default '0',
  `mt_amount` int(10) default '0',
  `mt_player` int(10) default '0',
  `mt_date` char(45) collate latin1_general_ci default '2750-01-01 00:00:00',
  `mt_comment` char(255) collate latin1_general_ci default '0',
  PRIMARY KEY  (`mt_id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

/*Table structure for table `neochronicle` */

DROP TABLE IF EXISTS `neochronicle`;

CREATE TABLE `neochronicle` (
  `nc_id` int(10) unsigned NOT NULL auto_increment,
  `nc_name` varchar(45) NOT NULL default '',
  `nc_content` text NOT NULL,
  `nc_datetime` varchar(45) NOT NULL default '',
  `nc_lang` int(2) default NULL,
  `nc_approved` int(1) default '0',
  `nc_author` char(45) default '',
  `nc_icon` int(10) default '0',
  PRIMARY KEY  (`nc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;

/*Table structure for table `neocron articles` */

DROP TABLE IF EXISTS `neocron articles`;

CREATE TABLE `neocron articles` (
  `na_id` int(10) unsigned NOT NULL auto_increment,
  `na_datetime` varchar(45) NOT NULL default '',
  `na_author` varchar(45) NOT NULL default '0',
  `na_name` varchar(45) NOT NULL default '',
  `na_content` text NOT NULL,
  `na_approval` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`na_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Table structure for table `npc_shop` */

DROP TABLE IF EXISTS `npc_shop`;

CREATE TABLE `npc_shop` (
  `c_npc_id` int(11) default NULL COMMENT 'The NPC WorldID',
  `c_zoneid` int(11) default NULL COMMENT 'The ZoneID in which the NPC is',
  `c_itemid` int(11) default NULL COMMENT 'ItemID for sale',
  `c_itemprice` int(11) default NULL COMMENT 'Price for this item'
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

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
) ENGINE=InnoDB AUTO_INCREMENT=9968 DEFAULT CHARSET=latin1;

/*Table structure for table `outposts` */

DROP TABLE IF EXISTS `outposts`;

CREATE TABLE `outposts` (
  `o_id` int(10) unsigned NOT NULL auto_increment,
  `o_outnum` int(10) unsigned NOT NULL default '0',
  `o_security` int(10) unsigned NOT NULL default '0',
  `o_clan` int(10) unsigned NOT NULL default '0',
  `o_gr` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`o_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2213 DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

/*Table structure for table `stockx` */

DROP TABLE IF EXISTS `stockx`;

CREATE TABLE `stockx` (
  `st_id` int(10) unsigned NOT NULL auto_increment,
  `st_factid` int(10) unsigned NOT NULL default '0',
  `st_curval` float NOT NULL default '0',
  `st_oldval` float NOT NULL default '0',
  PRIMARY KEY  (`st_id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=latin1;

/*Table structure for table `stockx_depots` */

DROP TABLE IF EXISTS `stockx_depots`;

CREATE TABLE `stockx_depots` (
  `sxd_id` int(10) NOT NULL auto_increment,
  `sxd_playerid` int(10) default '0',
  `sxd_st_id` int(10) default '0',
  `sxd_amount` int(10) default '0',
  `sxd_paid` int(10) default '0',
  PRIMARY KEY  (`sxd_id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

/*Table structure for table `support` */

DROP TABLE IF EXISTS `support`;

CREATE TABLE `support` (
  `su_id` int(10) NOT NULL auto_increment,
  `su_player` int(10) default '0',
  `su_datetime` varchar(45) collate latin1_general_ci default '',
  `su_supporterid` int(10) default '0',
  `su_desc` char(255) collate latin1_general_ci default '',
  `su_type` int(1) default '0',
  `su_worldid` int(10) default '0',
  `su_inwork` int(1) default '0',
  `su_finished` int(1) default '0',
  PRIMARY KEY  (`su_id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

/*Table structure for table `vehicles` */

DROP TABLE IF EXISTS `vehicles`;

CREATE TABLE `vehicles` (
  `v_id` int(10) unsigned NOT NULL auto_increment,
  `v_owner` int(10) unsigned NOT NULL default '0',
  `v_type` int(10) unsigned NOT NULL default '0',
  `v_condition` int(10) unsigned NOT NULL default '0',
  `v_status` int(10) unsigned NOT NULL default '0',
  `v_world` int(10) default '0',
  PRIMARY KEY  (`v_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

/*Table structure for table `world_actors` */

DROP TABLE IF EXISTS `world_actors`;

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
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

/*Table structure for table `worlds` */

DROP TABLE IF EXISTS `worlds`;

CREATE TABLE `worlds` (
  `wr_id` int(10) NOT NULL default '0',
  `wr_group` int(10) default '0',
  PRIMARY KEY  (`wr_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;


