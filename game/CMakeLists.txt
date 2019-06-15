cmake_minimum_required(VERSION 3.0)
PROJECT(Ackcon19)

# ANLEITUNG:
#
# Erstelle eine AckCon19.pri *neben* der AckCon19.pro
# und schreibe dort den Installationspfad deiner
# GStudio8-Installation.
#
# Anschließend gehe in QtCreator auf "Projects" und lösche dort alle
# Build-Schritte.
#
# Bei "Run" trage folgendes ein:
# Executable             = Pfad zur acknex.exe aus der GStudio8-Installation
# Command line arguments = main.c
# Working directory      = %{sourceDir}

# Configure your acknex installation here!
include(AckCon19.cmake)

if(NOT EXISTS "${ACKNEX_INSTALL_PATH}")
  message(FATAL_ERROR "Create AckCon19.cmake, and set ACKNEX_INSTALL_PATH in it to the installation of your gamestudio 8 installation.")
endif()

set(ACK19_SOURCES
	main.c
	src/framework.c
	src/game.c
	src/mainmenu.c
)

set(ACK19_HEADERS
	src/framework.h
	src/game.h
	src/global.h
	src/mainmenu.h
)

file(GLOB_RECURSE ACK19_SOURCES
 "${CMAKE_SOURCE_DIR}/*.c"
 "${CMAKE_SOURCE_DIR}/src/*.c"
 )

file(GLOB_RECURSE ACK19_HEADERS
 "${CMAKE_SOURCE_DIR}/src/*.h"
 )

add_executable(Ack19 "${ACK19_SOURCES}" "${ACK19_HEADERS}")

set(CURRENT_GAME_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(Ack19 PRIVATE ${ACKNEX_INSTALL_PATH}/include ${CURRENT_GAME_CMAKE_DIR}/src)
