cmake_minimum_required(VERSION 3.1)
project(drvrpcpp)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(AMXConfig)
include(AddSAMPPlugin)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/amx
)

add_definitions(-DSAMPGDK_AMALGAMATION)

add_samp_plugin(drvrp
  amxplugin.cpp
  drvrp.cpp
  drvrp.def
  streamer.hpp
  streamer.cpp
  streamer.def
  sampgdk.c
  sampgdk.h
)
