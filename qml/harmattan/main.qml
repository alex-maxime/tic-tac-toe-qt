/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: window

    initialPage: GamePage {
        id: gamePage
    }

    Component.onCompleted: {
        // Use the dark theme
        theme.inverted = true;
    }
}

// End of file.
