cmake_minimum_required(VERSION 3.28)
project(drvrpcpp)

set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(AMXConfig)
include(AddSAMPPlugin)

include_directories(
  ${SRC}
  ${SRC}/lib
  ${SRC}/lib/amx
)

add_compile_options(-Wall -Wextra -Wconversion -Wno-sign-conversion)
if (MINGW)
  add_compile_options(-Wno-maybe-uninitialized)
endif()
add_definitions(-DSAMPGDK_AMALGAMATION)
file(GLOB CMD_SOURCES CONFIGURE_DEPENDS "${SRC}/cmd/*.cpp")

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
  ${SRC}/lib/sha256.h
  ${SRC}/lib/sha256.cpp
  
  ${SRC}/lib/cmd.c
  ${SRC}/lib/stuff.cpp

  ${CMD_SOURCES}
)

set_source_files_properties(${SRC}/lib/streamer.cpp PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties(${SRC}/lib/sampgdk.c PROPERTIES COMPILE_FLAGS
  "-Wno-unused-parameter -Wno-strict-aliasing -Wno-sign-compare"
)
set_source_files_properties(${SRC}/lib/sha256.cpp PROPERTIES COMPILE_FLAGS -Wno-conversion)
