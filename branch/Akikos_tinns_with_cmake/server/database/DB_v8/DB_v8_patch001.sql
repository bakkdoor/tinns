use gameserver;

alter table `emails` change `e_body` `e_body` varchar(2048) character set latin1 collate latin1_swedish_ci NOT NULL;
