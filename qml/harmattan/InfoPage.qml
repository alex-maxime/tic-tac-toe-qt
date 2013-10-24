/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import "HarmattanUIConstants.js" as Constants
import "CommonUIConstants.js" as CommonConstants

Page {
    id: infoPage

    property Style platformStyle: LabelStyle {}

    Flickable {
        id: container

        anchors {
            fill: parent
            topMargin: Constants.DEFAULT_MARGIN
            leftMargin: Constants.DEFAULT_MARGIN
        }

        contentHeight: connInfoHeader.height
                       + connInfoGrid.height
                       + aboutHeader.height
                       + aboutContent.height
                       + 50;

        // Connection information header
        Text {
            id: connInfoHeader

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            height: font.pixelSize + Constants.DEFAULT_MARGIN
            color: Constants.TEXT_COLOR
            font.family: platformStyle.fontFamily
            font.pixelSize: platformStyle.fontPixelSize + 5
            font.bold: true
            text: CommonConstants.CONN_INFO_TITLE
        }

        // Connection information grid containing label and value pairs
        Grid {
            id: connInfoGrid
            height: ssidLabel.height * 2 + Constants.DEFAULT_MARGIN

            anchors {
                top: connInfoHeader.bottom
                left: parent.left
                right: parent.right
                rightMargin: Constants.DEFAULT_MARGIN
            }

            columns: 2
            spacing: Constants.DEFAULT_MARGIN

            Text {
                id: ssidLabel
                width: 150
                height: ssidTextField.height
                color: Constants.TEXT_COLOR
                verticalAlignment: Text.AlignVCenter
                font.family: platformStyle.fontFamily
                font.pixelSize: platformStyle.fontPixelSize
                text: "SSID"
            }
            TextField {
                id: ssidTextField
                width: parent.width - ssidLabel.width - Constants.DEFAULT_MARGIN
                enabled: false
                text: gameEngine.ssid
            }

            Text {
                width: ssidLabel.width
                height: ssidLabel.height
                color: Constants.TEXT_COLOR
                verticalAlignment: Text.AlignVCenter
                font.family: platformStyle.fontFamily
                font.pixelSize: platformStyle.fontPixelSize
                text: "IP and port"
            }
            TextField {
                width: ssidTextField.width
                enabled: false
                text: (gameEngine.ipAndPort === "") ?
                          "Not connected" : gameEngine.ipAndPort;
            }
        }

        // About header
        Text {
            id: aboutHeader

            anchors {
                top: connInfoGrid.bottom
                left: parent.left
                right: parent.right
                topMargin: Constants.DEFAULT_MARGIN * 3
            }

            height: font.pixelSize + Constants.DEFAULT_MARGIN
            color: Constants.TEXT_COLOR
            font.family: platformStyle.fontFamily
            font.pixelSize: platformStyle.fontPixelSize + 5
            font.bold: true
            text: CommonConstants.ABOUT_TITLE
        }

        // About information
        Text {
            id: aboutContent

            anchors {
                top: aboutHeader.bottom
                left: parent.left
                right: parent.right
                rightMargin: Constants.DEFAULT_MARGIN
            }

            color: Constants.TEXT_COLOR
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.family: platformStyle.fontFamily
            font.pixelSize: platformStyle.fontPixelSize
            text: CommonConstants.ABOUT_CONTENT
            textFormat: Text.RichText
            onLinkActivated: Qt.openUrlExternally(link);
        }
    }

    ScrollDecorator {
        flickableItem: container
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: pageStack.pop();
        }
    }
}

// End of file.
