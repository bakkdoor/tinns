-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	5.0.18-nt


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema infoserver
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ infoserver DEFAULT CHARACTER SET latin1 COLLATE latin1_general_ci;
USE infoserver;

--
-- Table structure for table `infoserver`.`accounts`
--

DROP TABLE IF EXISTS `accounts`;
CREATE TABLE `accounts` (
  `a_id` int(10) unsigned NOT NULL auto_increment,
  `a_username` varchar(45) NOT NULL default '',
  `a_password` varchar(45) NOT NULL default '',
  `a_priv` int(10) unsigned default '0',
  `a_status` int(10) unsigned default '0',
  `a_bandate` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`a_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `infoserver`.`accounts`
--

/*!40000 ALTER TABLE `accounts` DISABLE KEYS */;
/*!40000 ALTER TABLE `accounts` ENABLE KEYS */;


--
-- Table structure for table `infoserver`.`server_list`
--

DROP TABLE IF EXISTS `server_list`;
CREATE TABLE `server_list` (
  `s_id` int(10) unsigned NOT NULL auto_increment,
  `s_name` varchar(15) NOT NULL default 'NC server',
  `s_addr` varchar(15) NOT NULL default '127.0.0.1',
  `s_port` int(10) unsigned NOT NULL default '12000',
  `s_players` int(10) unsigned NOT NULL default '0',
  `s_lastupdate` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`s_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `infoserver`.`server_list`
--

/*!40000 ALTER TABLE `server_list` DISABLE KEYS */;
/*!40000 ALTER TABLE `server_list` ENABLE KEYS */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
