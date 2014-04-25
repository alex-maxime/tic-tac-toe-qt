/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

import QtQuick 1.0
import com.nokia.symbian 1.1
import TicTacToeGame 1.0
import "../common/"

Window {
    id: window

    // Application title
    Text {
        id: title
        text: "Tic-Tac-Toe"
        anchors.top: statusBar.top
        anchors.left: parent.left
        anchors.leftMargin: 10
        color: "white"
        z: 2
    }

    // Pages
    GamePage {
        id: gamePage
    }

    InfoPage {
        id: infoPage
    }

    // Common stuff: page stack, status bar and toolbar
    PageStack {
        id: pageStack

        anchors {
            left: parent.left
            right: parent.right
            top: statusBar.bottom
            bottom: theToolBar.top
        }

        toolBar: theToolBar
    }

    StatusBar {
        id: statusBar
        anchors.top: parent.top
    }

    ToolBar {
        id: theToolBar
        anchors.bottom: parent.bottom
    }

    Component.onCompleted: {
        pageStack.push(gamePage);
    }
}
