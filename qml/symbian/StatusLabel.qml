/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.0
import com.nokia.symbian 1.1
import "SymbianUIConstants.js" as Constants

Item {
    id: statusLabel

    property alias color: colorIndicator.color
    property alias text: statusText.text
    property alias value: progressBar.value

    // Default width and height
    width: 300
    height: 80

    ProgressBar {
        id: progressBar
        width: parent.width

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        opacity: 0
        minimumValue: 0
        maximumValue: 100

        Behavior on opacity { NumberAnimation { duration: 300 } }

        onValueChanged: {
            if (progressBar.value == progressBar.maximumValue) {
                // Hide the progress bar
                hideProgressBarTimer.restart();
            }
            else if (!progressBar.opacity) {
                progressBar.opacity = 1;
            }
        }

        Timer {
            id: hideProgressBarTimer
            interval: 3000
            repeat: false
            running: false
            onTriggered: progressBar.opacity = 0;
        }
    }

    Rectangle {
        id: colorIndicator
        width: 15
        height: parent.height - progressBar.height
                - Constants.DEFAULT_MARGIN / 2
        color: "white"

        anchors {
            top: progressBar.bottom
            topMargin: Constants.DEFAULT_MARGIN / 2
            left: parent.left
        }
    }

    Text {
        id: statusText
        width: parent.width - colorIndicator.width
        height: colorIndicator.height

        anchors {
            top: progressBar.bottom
            topMargin: Constants.DEFAULT_MARGIN / 2
            left: colorIndicator.right
            leftMargin: Constants.DEFAULT_MARGIN
        }

        color: platformStyle.colorNormalLight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: height / 2
    }
}

// End of file.
