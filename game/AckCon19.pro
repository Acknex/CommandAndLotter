TEMPLATE=aux

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
include(AckCon19.pri)

!defined(ACKNEX_INSTALL_PATH, var) {
  error("Create AckCon19.pri, and set ACKNEX_INSTALL_PATH in it to the installation of your gamestudio 8 installation.")
}

INCLUDEPATH = $$quote($$ACKNEX_INSTALL_PATH/include)
DEPENDPATH = $$quote($$ACKNEX_INSTALL_PATH/include)

INCLUDEPATH += $$pwd/src
DEPENDPATH += $$pwd/src

SOURCES += \
  main.c \
  src/ai.c \
  src/ang.c \
  src/building.c \
  src/camera.c \
  src/credits.c \
  src/effects2d.c \
  src/enemy_hit.c \
  src/framework.c \
  src/game.c \
  src/ini.c \
  src/jps.c \
  src/mainmenu.c \
  src/map_loader.c \
  src/UnitMangement.c \
  src/music_player.c \
  src/progressbars.c \
  src/scan.c \
  src/settings.c \
  src/sputnik.c \
  src/stub.c \
  src/ui_game.c \
  src/ui_menu.c \
  src/unit.c \
    src/ang.c \
    src/camera.c \
    src/credits.c \
    src/enemy_hit.c \
    src/framework.c \
    src/game.c \
    src/grid.c \
    src/mainmenu.c \
    src/map_loader.c \
    src/music_player.c \
    src/particle.c \
    src/scan.c \
    src/spawner.c \
    src/sputnik.c \
    src/stub.c \
    src/ui_game.c \
    src/unit.c \
    src/UnitMangement.c \
    src/fow.c\
    main.c \
  src/z.c

HEADERS += \
  shaders/terrain.fx \
  src/ai.h \
  src/ang.h \
  src/building.h \
  src/camera.h \
  src/console.h \
  src/credits.h \
  src/effects2d.h \
  src/enemy_hit.h \
  src/fow.h \
  src/framework.h \
  src/game.h \
  src/global.h \
  src/ini.h \
  src/jps.h \
  src/mainmenu.h \
  src/map_loader.h \
  src/UnitMangement.h \
  src/materials.h \
  src/music_player.h \
  src/progressbars.h \
  src/scan.h \
  src/settings.h \
  src/sputnik.h \
  src/stub.h \
  src/ui_game.h \
  src/ui_menu.h \
  src/unit.h \
    src/ang.h \
    src/camera.h \
    src/credits.h \
    src/enemy_hit.h \
    src/framework.h \
    src/game.h \
    src/global.h \
    src/grid.h \
    src/mainmenu.h \
    src/map_loader.h \
    src/music_player.h \
    src/particle.h \
    src/scan.h \
    src/spawner.h \
    src/sputnik.h \
    src/stub.h \
    src/ui_game.h \
    src/unit.h \
    src/UnitMangement.h \
    src/fow.h\ \
  src/z.h

DISTFILES += \
  main.wdl \
  shaders/progress-radial.fx \
  shaders/water.fx
