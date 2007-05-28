-- phpMyAdmin SQL Dump
-- version 2.8.1
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Sep 27, 2006 at 08:37 PM
-- Server version: 4.1.14
-- PHP Version: 4.4.0
-- 
-- Database: `gameserver`
--
-- Additions from version 4 to version 5
-- To be applied on a gameserver database from GameDB5.sql
--
USE `gameserver`;

-- --------------------------------------------------------

-- 
-- Table structure addition for table `npc_spawns`
-- 

ALTER TABLE `npc_spawns`
ADD `npc_customname` VARCHAR( 80 ) NULL ,
ADD `npc_customscript` VARCHAR( 100 ) NULL ;


-- --------------------------------------------------------

-- 
-- Table addition: `world_actors`
-- 

CREATE TABLE IF NOT EXISTS `world_actors` (
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
-- Optionnal: Use the following lines with TinNS ONLY (uncomment them)
-- It doesn't matter if you don't use them
-- 

-- TRUNCATE TABLE 'apt_doors';
-- TRUNCATE TABLE 'apt_items';
-- TRUNCATE TABLE 'world_doors';
-- TRUNCATE TABLE 'world_items';
