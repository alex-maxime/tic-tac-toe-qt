/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.0
import TicTacToeGame 1.0
import "CommonUIConstants.js" as Constants

Item {
    id: ticTacGrid

    Grid {
        id: grid
        property int ticTacWidth: (parent.width - Constants.DEFAULT_MARGIN * (rows - 1)) / rows
        property int ticTacHeight: (parent.height - Constants.DEFAULT_MARGIN * (columns - 1)) / columns

        spacing: Constants.DEFAULT_MARGIN
        anchors.fill: parent
        rows: rowCount
        columns: columnCount

        Repeater {
            model: gameModel

            delegate: TicTac {
                id: ticTac
                width: grid.ticTacWidth
                height: grid.ticTacHeight
                modelValue: mark
                hilighted: highlight

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        if (ticTac.modelValue == GameModel.None
                                && gameEngine.status == GameEngine.WaitUser) {
                            gameEngine.mark(index);
                        }
                        else {
                            console.debug("Invalid selection!");
                            invalidSelectionAnimation.restart();
                        }
                    }
                }

                SequentialAnimation {
                    id: invalidSelectionAnimation
                    ColorAnimation {
                        target: ticTac
                        property: "color"
                        to: "red";
                        duration: Constants.DEFAULT_ANIMATION_DURATION
                    }
                    ColorAnimation {
                        target: ticTac
                        property: "color"
                        to: ticTac.hilighted ? Constants.TIC_TAC_HILIGHT_COLOR
                                             : Constants.TIC_TAC_BACKGROUND_COLOR;
                        duration: Constants.DEFAULT_ANIMATION_DURATION
                    }
                }
            } // delegate: TicTac
        } // Repeater
    } // Grid
}

// End of file.
