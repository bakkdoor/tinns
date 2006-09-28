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
-- To be applied on a gameserver database from GameDB4.sql
--
USE `gameserver`;

-- --------------------------------------------------------

-- 
-- Table structure addition for table `apt_items`
-- 

ALTER TABLE `apt_items` ADD `ai_option3` INT( 10 ) UNSIGNED NOT NULL DEFAULT '0';

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
-- Table structure addition for table `world_items`
-- 

ALTER TABLE `world_items` ADD `wi_option3` INT(10) UNSIGNED NOT NULL DEFAULT '0';

-- --------------------------------------------------------
