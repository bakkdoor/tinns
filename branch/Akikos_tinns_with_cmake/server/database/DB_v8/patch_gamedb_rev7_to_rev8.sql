/* Create table in target */
DROP TABLE `bug report`;

CREATE TABLE `bug_report`(
   `br_id` int(10) unsigned NOT NULL  auto_increment , 
   `br_type` int(10) unsigned NOT NULL  DEFAULT '0' COMMENT 'Type' , 
   `br_desc` varchar(1024) COLLATE latin1_swedish_ci NOT NULL  COMMENT 'Description' , 
   `br_fromid` int(10) unsigned NOT NULL  DEFAULT '0' COMMENT 'Char ID of reporter' , 
   `br_location` int(10) unsigned NOT NULL  DEFAULT '0' COMMENT 'Location where problem occured' , 
   `br_status` int(10) unsigned NOT NULL  DEFAULT '0' COMMENT 'Status' , 
   `br_datetime` varchar(45) COLLATE latin1_swedish_ci NOT NULL  COMMENT 'Date/Time' , 
   `br_supid` int(10) unsigned NOT NULL  DEFAULT '0' COMMENT 'Supporter ID' , 
   PRIMARY KEY (`br_id`) 
) ENGINE=InnoDB DEFAULT CHARSET='latin1';


/* Alter table in target */
ALTER TABLE `characters` 
   ADD COLUMN `c_online` int(1)   NULL DEFAULT '0' after `c_slot`, 
   ADD COLUMN `c_clan` int(10)   NULL DEFAULT '0' after `c_online`, 
   ADD COLUMN `c_soullight` int(3)   NULL DEFAULT '10' after `c_clan`, COMMENT='';

/* Create table in target */
CREATE TABLE `clanlevels`(
   `cll_id` int(10) NOT NULL  auto_increment , 
   `cll_clanid` int(10) NULL  DEFAULT '0' , 
   `cll_level` int(2) NULL  DEFAULT '0' , 
   `cll_desc` char(255) COLLATE latin1_general_ci NULL  , 
   `cll_charid` int(10) NULL  , 
   PRIMARY KEY (`cll_id`) 
) ENGINE=MyISAM DEFAULT CHARSET='latin1';


/* Alter table in target */
ALTER TABLE `clans` 
   ADD COLUMN `cl_id` int(10) unsigned   NOT NULL auto_increment first, 
   ADD COLUMN `cl_shortdesc` char(16)  COLLATE latin1_general_ci NULL after `cl_id`, 
   ADD COLUMN `cl_name` char(64)  COLLATE latin1_general_ci NULL after `cl_shortdesc`, 
   ADD COLUMN `cl_faction` int(2)   NULL after `cl_name`, 
   ADD COLUMN `cl_description` varchar(256)  COLLATE latin1_general_ci NULL after `cl_faction`, 
   ADD COLUMN `cl_leader` int(10)   NULL DEFAULT '0' after `cl_description`, 
   ADD COLUMN `cl_money` int(10)   NULL DEFAULT '0' after `cl_leader`, 
   ADD COLUMN `cl_minsympathy` int(3)   NULL DEFAULT '70' after `cl_money`, 
   ADD COLUMN `cl_appid` int(10)   NULL DEFAULT '0' after `cl_minsympathy`, 
   ADD COLUMN `cl_representative` int(10)   NULL DEFAULT '0' after `cl_appid`, 
   DROP COLUMN `c_id`, 
   DROP COLUMN `c_shortname`, 
   DROP COLUMN `c_longname`, 
   DROP COLUMN `c_faction`, 
   DROP KEY `PRIMARY`, add PRIMARY KEY(`cl_id`), ENGINE=MyISAM, COMMENT='';

/* Create table in target */
CREATE TABLE `clanwars`(
   `cw_id` int(10) NOT NULL  auto_increment , 
   `cw_initclan` int(10) NULL  DEFAULT '0' , 
   `cw_enemyclan` int(10) NULL  DEFAULT '0' , 
   `cw_starttime` varchar(45) COLLATE latin1_general_ci NULL  , 
   `cw_status` int(2) NULL  DEFAULT '0' , 
   `cw_statement_initiator` varchar(512) COLLATE latin1_general_ci NULL  , 
   `cw_statement_enemy` varchar(512) COLLATE latin1_general_ci NULL  , 
   PRIMARY KEY (`cw_id`) 
) ENGINE=MyISAM DEFAULT CHARSET='latin1';


/* Create table in target */
CREATE TABLE `datacubes`(
   `cb_id` int(10) NOT NULL  auto_increment , 
   `cb_securitycode` char(30) COLLATE latin1_general_ci NULL  , 
   `cb_inscription` tinytext COLLATE latin1_general_ci NULL  , 
   PRIMARY KEY (`cb_id`) 
) ENGINE=MyISAM DEFAULT CHARSET='latin1';


/* Alter table in target */
ALTER TABLE `forum_posts` 
   CHANGE `fp_content` `fp_content` varchar(2048)  COLLATE latin1_swedish_ci NOT NULL after `fp_fromid`, 
   ADD COLUMN `fp_factionid` int(2)   NULL DEFAULT '0' after `fp_forumid`, 
   ADD COLUMN `fp_clanid` int(10)   NULL DEFAULT '0' after `fp_factionid`, COMMENT='';

/* Alter table in target */
ALTER TABLE `guides` 
   ADD COLUMN `g_language` int(2)   NULL after `g_title`, 
   CHANGE `g_content` `g_content` text  COLLATE latin1_swedish_ci NOT NULL after `g_language`, 
   ADD COLUMN `g_chapter` int(1)   NULL DEFAULT '0' after `g_content`, 
   ADD COLUMN `g_part` int(1)   NULL DEFAULT '0' after `g_chapter`, COMMENT='';

/* Create table in target */
CREATE TABLE `moneytransactions`(
   `mt_id` int(10) NOT NULL  auto_increment , 
   `mt_clanid` int(10) NULL  DEFAULT '0' , 
   `mt_amount` int(10) NULL  DEFAULT '0' , 
   `mt_player` int(10) NULL  DEFAULT '0' , 
   `mt_date` char(45) COLLATE latin1_general_ci NULL  DEFAULT '2750-01-01 00:00:00' , 
   `mt_comment` char(255) COLLATE latin1_general_ci NULL  DEFAULT '0' , 
   PRIMARY KEY (`mt_id`) 
) ENGINE=MyISAM DEFAULT CHARSET='latin1';


/* Alter table in target */
ALTER TABLE `neochronicle` 
   CHANGE `nc_content` `nc_content` text  COLLATE latin1_swedish_ci NOT NULL after `nc_name`, 
   ADD COLUMN `nc_lang` int(2)   NULL after `nc_datetime`, 
   ADD COLUMN `nc_approved` int(1)   NULL DEFAULT '0' after `nc_lang`, 
   ADD COLUMN `nc_author` char(45)  COLLATE latin1_swedish_ci NULL after `nc_approved`, 
   ADD COLUMN `nc_icon` int(10)   NULL DEFAULT '0' after `nc_author`, COMMENT='';

/* Alter table in target */
ALTER TABLE `neocron articles` 
   CHANGE `na_content` `na_content` text  COLLATE latin1_swedish_ci NOT NULL after `na_name`, COMMENT='';

/* Create table in target */
CREATE TABLE `npc_shop`(
   `c_npc_id` int(11) NULL  COMMENT 'The NPC WorldID' , 
   `c_zoneid` int(11) NULL  COMMENT 'The ZoneID in which the NPC is' , 
   `c_itemid` int(11) NULL  COMMENT 'ItemID for sale' , 
   `c_itemprice` int(11) NULL  COMMENT 'Price for this item' 
) ENGINE=MyISAM DEFAULT CHARSET='latin1';


/* Alter table in target */
ALTER TABLE `npc_spawns` 
   ADD COLUMN `npc_unknown` int(10) unsigned   NOT NULL DEFAULT '0' after `npc_loot`, 
   ADD COLUMN `npc_trader` int(10) unsigned   NOT NULL DEFAULT '0' after `npc_unknown`, 
   ADD COLUMN `npc_customname` varchar(80)  COLLATE latin1_swedish_ci NULL after `npc_trader`, 
   ADD COLUMN `npc_customscript` varchar(100)  COLLATE latin1_swedish_ci NULL after `npc_customname`, 
   ADD COLUMN `npc_shop_quality` int(3)   NULL DEFAULT '50' COMMENT 'The quality of items if this npc has items to sell' after `npc_customscript`, 
   ADD COLUMN `npc_scripting` int(1)   NULL DEFAULT '1' COMMENT '1: Scripts enabled 0: Scripts disabled' after `npc_shop_quality`, COMMENT='';

/* Create table in target */
CREATE TABLE `stockx_depots`(
   `sxd_id` int(10) NOT NULL  auto_increment , 
   `sxd_playerid` int(10) NULL  DEFAULT '0' , 
   `sxd_st_id` int(10) NULL  DEFAULT '0' , 
   `sxd_amount` int(10) NULL  DEFAULT '0' , 
   `sxd_paid` int(10) NULL  DEFAULT '0' , 
   PRIMARY KEY (`sxd_id`) 
) ENGINE=MyISAM DEFAULT CHARSET='latin1';


/* Create table in target */
CREATE TABLE `support`(
   `su_id` int(10) NOT NULL  auto_increment , 
   `su_player` int(10) NULL  DEFAULT '0' , 
   `su_datetime` varchar(45) COLLATE latin1_general_ci NULL  , 
   `su_supporterid` int(10) NULL  DEFAULT '0' , 
   `su_desc` char(255) COLLATE latin1_general_ci NULL  , 
   `su_type` int(1) NULL  DEFAULT '0' , 
   `su_worldid` int(10) NULL  DEFAULT '0' , 
   `su_inwork` int(1) NULL  DEFAULT '0' , 
   `su_finished` int(1) NULL  DEFAULT '0' , 
   PRIMARY KEY (`su_id`) 
) ENGINE=MyISAM DEFAULT CHARSET='latin1';


/* Alter table in target */
ALTER TABLE `vehicles` 
   ADD COLUMN `v_world` int(10)   NULL DEFAULT '0' after `v_status`, COMMENT='';

/* Create table in target */
CREATE TABLE `worlds`(
   `wr_id` int(10) NOT NULL  DEFAULT '0' , 
   `wr_group` int(10) NULL  DEFAULT '0' , 
   PRIMARY KEY (`wr_id`) 
) ENGINE=MyISAM DEFAULT CHARSET='latin1';
