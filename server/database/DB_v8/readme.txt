This directory contains an UPDATE and a FULL DUMP
for both InfoDB and GameDB.
If you want to keep your data and just upgrade the structure,
use
	patch_gamedb_rev7_to_rev8.sql

to get up to date. However, it is recommended to use the FULL files
to build a new database.

Note: For TinNS, you'll have to apply "TinNS_GameDB_rev8_BaseData.sql"
after creating the base-tables!

See changes.txt for information about what has changed from DB7 to DB8