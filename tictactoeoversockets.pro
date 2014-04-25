# Copyright (c) 2011-2014 Microsoft Mobile.

QT += network declarative
CONFIG += qt-components
TARGET = TicTacToe
VERSION = 1.1.1

# Additional import path used to resolve QML modules in Creator's code model
#QML_IMPORT_PATH =

HEADERS += \
    src/connectionmanager.h \
    src/gamecommunicator.h \
    src/gamemodel.h \
    src/loadhelper.h \
    src/protocolutility.h \
    src/tictacclient.h \
    src/tictacengine.h \
    src/tictacserver.h

SOURCES += \
    src/main.cpp \
    src/connectionmanager.cpp \
    src/gamemodel.cpp \
    src/protocolutility.cpp \
    src/tictacclient.cpp \
    src/tictacengine.cpp \
    src/tictacserver.cpp

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    qml/common/SplashScreen.qml \
    qml/common/TicTac.qml \
    qml/common/TicTacGrid.qml \
    qml/common/CommonUIConstants.js

symbian {
    TARGET.UID3 = 0xECB5DBD7

    # Allow network access on Symbian
    TARGET.CAPABILITY += NetworkServices
    #ReadUserData

    LIBS += -lcone -leikcore -lavkon

    # Add files/folders to ship with the application, here
    folder_01.sources = qml/symbian
    folder_01.path = .
    folder_02.sources = qml/common
    folder_02.path = .
    folder_03.sources = gfx
    folder_03.path = .
    DEPLOYMENT += folder_01 folder_02 folder_03

    ICON = icons/tictactoe.svg

    OTHER_FILES += qml/symbian/*

    # Smart Installer package's UID
    # This UID is from the protected range and therefore the package will
    # fail to install if self-signed. By default qmake uses the unprotected
    # range value if unprotected UID is defined for the application and
    # 0x2002CCCF value if protected UID is given to the application
    #DEPLOYMENT.installer_header = 0x2002CCCF
}

contains(MEEGO_EDITION, harmattan) {
    TARGET = tictactoeoversockets

    RESOURCES += harmattan.qrc

    OTHER_FILES += \
        qml/harmattan/main.qml \
        qml/harmattan/InfoPage.qml \
        qml/harmattan/GamePage.qml \
        qml/harmattan/StatusLabel.qml \
        qml/harmattan/HarmattanUIConstants.js

    target.path = /opt/$${TARGET}/bin
    desktopfile.files = $${TARGET}.desktop
    desktopfile.path = /usr/share/applications
    icon64.files += icons/tictactoe.png
    icon64.path = /usr/share/icons/hicolor/64x64/apps

    INSTALLS += \
        target \
        desktopfile \
        icon64
}

simulator {
    OTHER_FILES += qml/symbian/*

    # Modify the following path if necessary
    SHADOW_BLD_PATH = ..\\tictactoeoversockets-build-simulator-Simulator_Qt_for_MinGW_4_4__Qt_SDK__Debug

    # Copy the QML files to the shadow build folder
    system(mkdir $${SHADOW_BLD_PATH}\\common)
    system(mkdir $${SHADOW_BLD_PATH}\\symbian)
    system(copy qml\\common\\*.* $${SHADOW_BLD_PATH}\\common)
    system(copy qml\\symbian\\*.* $${SHADOW_BLD_PATH}\\symbian)
}



