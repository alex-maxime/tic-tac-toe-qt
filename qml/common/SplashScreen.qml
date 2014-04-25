/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

import QtQuick 1.0

Rectangle {
    id: splashScreen

    /*width: 360
    height: 640*/

    gradient: Gradient {
        GradientStop { position: 0; color: "#3d8bda"; }
        GradientStop {
            position: (splashScreen.height / 2 - icon.height / 2) / splashScreen.height
            color: "#3d8bda"
        }
        GradientStop {
            position: (splashScreen.height / 2 + icon.height / 3.5) / splashScreen.height
            color: "#1938a7"
        }
        GradientStop { position: 1; color: "#1938a7"; }
    }

    Image {
        id: icon
        anchors.centerIn: parent
        source: "qrc:/splash-screen-icon.png"
    }
}

// End of file.
