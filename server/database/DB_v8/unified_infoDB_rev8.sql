/*
SQLyog Enterprise - MySQL GUI v7.1 
MySQL - 5.0.51a-24+lenny2 : Database - infoserver
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE DATABASE /*!32312 IF NOT EXISTS*/`infoserver` /*!40100 DEFAULT CHARACTER SET latin1 COLLATE latin1_general_ci */;

USE `infoserver`;

/*Table structure for table `accounts` */

DROP TABLE IF EXISTS `accounts`;

CREATE TABLE `accounts` (
  `a_id` int(10) unsigned NOT NULL auto_increment,
  `a_username` varchar(45) NOT NULL default '',
  `a_password` varchar(45) NOT NULL default '',
  `a_priv` int(10) unsigned default '0',
  `a_status` int(10) unsigned default '0',
  `a_bandate` int(11) unsigned NOT NULL default '0',
  `a_emailaddress` varchar(255) NOT NULL,
  `a_creationdate` datetime NOT NULL default '0000-00-00 00:00:00',
  `a_lastused` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`a_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

/*Table structure for table `server_list` */

DROP TABLE IF EXISTS `server_list`;

CREATE TABLE `server_list` (
  `s_id` int(10) unsigned NOT NULL auto_increment,
  `s_name` varchar(15) NOT NULL default 'NC server',
  `s_wanaddr` varchar(15) NOT NULL default '127.0.0.1',
  `s_port` int(10) unsigned NOT NULL default '12000',
  `s_players` int(10) unsigned NOT NULL default '0',
  `s_lastupdate` datetime NOT NULL default '0000-00-00 00:00:00',
  `s_lanaddr` varchar(15) NOT NULL default '127.0.0.1',
  PRIMARY KEY  (`s_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
