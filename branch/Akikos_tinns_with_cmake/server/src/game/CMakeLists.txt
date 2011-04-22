include_directories     (${TINNS_SOURCE_DIR}/server/src/game)

#add_subdirectory	(decoder)
#add_subdirectory	(def)
#add_subdirectory	(gamecommands)

add_executable		(gameserver

			decoder/msgdecoder.cpp decoder/udp_0x13.cpp
			decoder/udp_0x1f.cpp decoder/udp_0x22.cpp
			decoder/udp_0x2b.cpp decoder/udpanalyser.cpp
			decoder/udp_appartment.cpp decoder/udp_charmove.cpp
			decoder/udp_chat.cpp decoder/udp_deathrespawn.cpp
			decoder/udp_entityposreq.cpp decoder/udp_hack.cpp
			decoder/udp_helditemaction.cpp decoder/udp_0x08.cpp
			decoder/udp_itemmanualreload.cpp
			decoder/udp_itemmove.cpp decoder/udp_itemuse.cpp
			decoder/udp_killself.cpp decoder/udp_OOO.cpp
			decoder/udp_outfitter.cpp decoder/udp_packet0.cpp
			decoder/udp_ping.cpp decoder/udp_popupresponse.cpp
			decoder/udp_quickaccessbelt.cpp decoder/udp_reqinfo.cpp
			decoder/udp_subskill.cpp decoder/udp_sync.cpp
			decoder/udp_terminal.cpp decoder/udp_useobject.cpp
			decoder/udp_vhc.cpp decoder/udp_zoning.cpp
			decoder/udp_worldIDinfo.cpp decoder/udp_multipart.cpp
			decoder/udp_pvptrade.cpp decoder/udp_npcdialog.cpp

			def/def_actionmod.cpp def/def_ammo.cpp
			def/def_appartements.cpp def/def_appplaces.cpp
			def/def_blueprintpieces.cpp def/def_characters.cpp
			def/def_charaction.cpp def/def_charkinds.cpp
			def/def_damage.cpp def/def_drugs.cpp
			def/def_factions.cpp def/def_hack.cpp
			def/def_implants.cpp def/def_itemcontainer.cpp
			def/def_itemmod.cpp def/def_itemres.cpp
			def/def_items.cpp def/def_mission.cpp
			def/def_npcarmor.cpp def/def_npc.cpp
			def/def_npcgroupspawn.cpp def/def_outposts.cpp
			def/defparser.cpp def/def_recycles.cpp
			def/def_respawn.cpp def/def_shots.cpp def/def_skills.cpp
			def/def_subskills.cpp def/def_trader.cpp
			def/def_vehicles.cpp def/def_vehiclesits.cpp
			def/def_weapons.cpp def/def_weather.cpp
			def/def_worldfile.cpp def/def_worldmodels.cpp
			def/def_worlds.cpp def/gamedefs.cpp
			def/world_datparser.cpp def/def_scripts.cpp

			gamecommands/ban.cpp gamecommands/brightness.cpp
			gamecommands/broadcast.cpp gamecommands/color.cpp
			gamecommands/debug.cpp gamecommands/effect.cpp
			gamecommands/givemoney.cpp gamecommands/h.cpp
			gamecommands/info.cpp gamecommands/npc_shop.cpp
			gamecommands/jail.cpp gamecommands/kick.cpp
			gamecommands/listbans.cpp gamecommands/online.cpp
			gamecommands/rawf.cpp gamecommands/recall.cpp
			gamecommands/remove.cpp gamecommands/setlevel.cpp
			gamecommands/settime.cpp gamecommands/setsubskill.cpp
			gamecommands/shun.cpp gamecommands/setmainskill.cpp
			gamecommands/skin.cpp gamecommands/spawnactor.cpp
			gamecommands/speed.cpp gamecommands/takemoney.cpp
			gamecommands/t.cpp gamecommands/teleport.cpp
			gamecommands/test.cpp gamecommands/unban.cpp
			gamecommands/unjail.cpp gamecommands/unshun.cpp
			gamecommands/uptime.cpp gamecommands/v.cpp
			gamecommands/version.cpp gamecommands/warp.cpp
			gamecommands/warpto.cpp gamecommands/weather.cpp
			gamecommands/npc.cpp

			globals.cpp terminal_querydb.cpp multipart.cpp
			accounts.cpp appartements.cpp buddylist.cpp
			chars.cpp chat.cpp client.cpp clientmanager.cpp
			commands.cpp container.cpp doortemplate.cpp
			furnituretemplate.cpp gameserver.cpp genreplist.cpp
			inventory.cpp isc.cpp item.cpp main.cpp lua_engine.cpp
			msgbuilder.cpp npc.cpp server.cpp skill.cpp sql.cpp
			subway.cpp terminal.cpp terminal_receivedb.cpp
			terminal_tryaccess.cpp npc_conversation.cpp
			terminal_updatedb.cpp vehicle.cpp vhcaccessrequest.cpp
			worldactors.cpp worlddatatemplate.cpp worlds.cpp
			zoning.cpp outpost.cpp npc_ai.cpp npctemplate.cpp)

target_link_libraries	(gameserver tinns ${MYSQL_LIBRARY} ${PCRE_LIBRARY}
		${ZLIB_LIBRARY} ${RT_LIBRARY} ${LUA_LIBRARY})
