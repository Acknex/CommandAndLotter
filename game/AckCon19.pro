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

SOURCES += \
  main.c
