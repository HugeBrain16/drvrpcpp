cmake_minimum_required(VERSION 3.28)
project(drvrpcpp)

set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(AMXConfig)
include(AddSAMPPlugin)

include_directories(
  ${SRC}
  ${SRC}/lib
  ${SRC}/lib/amx
)

add_definitions(-DSAMPGDK_AMALGAMATION)

add_samp_plugin(drvrp
  ${SRC}/drvrp.cpp
  ${SRC}/drvrp.def
  
  ${SRC}/player.cpp
  ${SRC}/vehicle.cpp
  ${SRC}/item.cpp
  ${SRC}/property.cpp
  ${SRC}/utils.cpp
  ${SRC}/statics.cpp

  ${SRC}/lib/amxplugin.cpp
  ${SRC}/lib/streamer.hpp
  ${SRC}/lib/streamer.cpp
  ${SRC}/lib/streamer.def
  ${SRC}/lib/sampgdk.c
  ${SRC}/lib/sampgdk.h
  
  ${SRC}/lib/cmd.c
  ${SRC}/lib/stuff.cpp

  ${SRC}/cmd/ssveh.cpp
  ${SRC}/cmd/getmaterials.cpp
  ${SRC}/cmd/storage.cpp
  ${SRC}/cmd/rconweapon.cpp
  ${SRC}/cmd/setskin.cpp
  ${SRC}/cmd/money.cpp
  ${SRC}/cmd/tmt.cpp
  ${SRC}/cmd/beadmin.cpp
  ${SRC}/cmd/noadmin.cpp
  ${SRC}/cmd/job.cpp
  ${SRC}/cmd/me.cpp
  ${SRC}/cmd/do.cpp
  ${SRC}/cmd/sweeper.cpp
  ${SRC}/cmd/engine.cpp
  ${SRC}/cmd/rentveh.cpp
  ${SRC}/cmd/lockrv.cpp
  ${SRC}/cmd/mower.cpp
  ${SRC}/cmd/createhouse.cpp
  ${SRC}/cmd/createbiz.cpp
  ${SRC}/cmd/enter.cpp
  ${SRC}/cmd/exit.cpp
  ${SRC}/cmd/buy.cpp
  ${SRC}/cmd/inventory.cpp
  ${SRC}/cmd/buyhouse.cpp
  ${SRC}/cmd/lockhouse.cpp
  ${SRC}/cmd/saveplayer.cpp
  ${SRC}/cmd/speedlimit.cpp
)
