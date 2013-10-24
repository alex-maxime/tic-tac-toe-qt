/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import TicTacToeGame 1.0
import "HarmattanUIConstants.js" as Constants

Page {
    id: root

    property bool inPortrait: window.inPortrait
    property int gameState: gameEngine.status
    property int userMark: gameEngine.userMark
    property string markAsString: (userMark === GameModel.Cross) ? "X" : "O"

    property Style platformStyle: LabelStyle {}

    Component.onCompleted: gameEngine.startGame();

    onGameStateChanged: updateItems();

    /**
     * Updates the UI according to the new game state.
     */
    function updateItems()
    {
        if (progressBarAnimation.running) {
            progressBarAnimation.stop();
            statusIndicator.value = 100;
        }

        switch (gameState) {
        case GameEngine.Started:
            progressBarAnimation.start();
            statusIndicator.text = qsTr("Searching for opponent...");
            statusIndicator.color = "white";
            break;

        case GameEngine.WaitUser:
            statusIndicator.text = qsTr("Your turn");
            statusIndicator.color = "lightgreen";
            break;

        case GameEngine.WaitOpponent:
            statusIndicator.text = qsTr("Waiting for opponent");
            statusIndicator.color = "yellow";
            break;

        case GameEngine.GameOver:
            var winnerId = gameEngine.winner();
            var outcome = "Draw!";

            if (winnerId != GameModel.None) {
                outcome = (winnerId === gameEngine.userMark) ?
                            "You win" : "You lost";
            }

            statusIndicator.text = outcome;
            statusIndicator.color = "white";
            break;
        case GameEngine.ConnectionError:
            statusIndicator.text = qsTr("Connection error");
            statusIndicator.color = "red";
            break;
        }
    }

    StatusLabel {
        id: statusIndicator

        width: root.inPortrait ?
                   parent.width - Constants.DEFAULT_MARGIN * 2
                 : parent.width / 2;
        height: 80

        anchors {
            top: parent.top
            topMargin: root.inPortrait ?
                           Constants.DEFAULT_MARGIN
                         : parent.height / 3 - height;
            left: parent.left
            leftMargin: Constants.DEFAULT_MARGIN / 2
        }

        PropertyAnimation {
            id: progressBarAnimation
            target: statusIndicator
            property: "value"
            from: 0; to: 100;
            duration: 30000 // 30 second timeout

            onCompleted: {
                if (root.gameState == GameEngine.Started) {
                    statusIndicator.text = qsTr("No opponent around!");
                    statusIndicator.color = "red";
                    gameEngine.terminateConnection();
                }
            }
        }
    }

    TicTacGrid {
        id: gameBoard
        width: root.inPortrait ?
                   parent.width - Constants.DEFAULT_MARGIN * 2
                 : parent.height - Constants.DEFAULT_MARGIN * 2;
        height: width

        anchors {
            bottom: parent.bottom;
            bottomMargin: root.inPortrait ?
                              parent.height / 2 - height / 2
                            : Constants.DEFAULT_MARGIN;
            right: parent.right
            rightMargin: Constants.DEFAULT_MARGIN
        }
    }

    Text {
        id: markIndicator

        anchors {
            left: parent.left
            leftMargin: root.inPortrait ?
                            parent.width / 2 - width / 2
                          : parent.width / 4 - width / 2;
            bottom: parent.bottom
            bottomMargin: root.inPortrait ? 20 :
                            parent.height / 2 - height - Constants.DEFAULT_MARGIN * 2;
        }

        color: Constants.TEXT_COLOR
        opacity: (gameState == GameEngine.WaitUser
                  || gameState == GameEngine.WaitOpponent
                  || gameState == GameEngine.GameOver) ? 1 : 0;

        Behavior on opacity { NumberAnimation { duration: 500 } }

        font.family: platformStyle.fontFamily
        font.pixelSize: platformStyle.fontPixelSize + 5
        text: "Your mark is '" + markAsString + "'"
    }

    // Info page
    InfoPage {
        id: infoPage
    }

    tools: ToolBarLayout {
        ToolIcon { } // Placeholder for back button
        ToolIcon {
            iconId: "toolbar-refresh"
            onClicked: gameEngine.startGame();
        }
        ToolIcon {
            iconSource: "qrc:///info.png"
            onClicked: pageStack.push(infoPage);
        }
    }
}

// End of file.
