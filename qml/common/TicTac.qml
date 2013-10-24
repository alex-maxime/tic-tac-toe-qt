/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.0
import TicTacToeGame 1.0
import "CommonUIConstants.js" as Constants

Rectangle {
    id: ticTac

    property int modelValue: -1
    property bool hilighted: false

    onModelValueChanged: {
        if (modelValue == GameModel.Zero) {
            ticTacText.text = "O";
        }
        else if (modelValue == GameModel.Cross) {
            ticTacText.text = "X";
        }
        else {
            ticTacText.text = "";
        }
    }

    width: Constants.TIC_TAC_DEFAULT_WIDTH
    height: width
    color: Constants.TIC_TAC_BACKGROUND_COLOR

    Text {
        id: ticTacText
        anchors.centerIn: parent
        color: hilighted ? Constants.TIC_TAC_HILIGHT_COLOR
                         : Constants.TIC_TAC_FOREGROUND_COLOR;
        font.pixelSize: parent.height - Constants.DEFAULT_MARGIN
    }
}

// End of file.
