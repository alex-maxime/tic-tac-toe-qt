Tic-Tac-Toe
===========

Tic-Tac-Toe is a Nokia Developer example application that is compatible with
the Windows Phone example 'Tic-Tac-Toe Over Sockets Sample'. This two-player
game communicates with another device over TCP sockets.

The application logic is implemented with Qt/C++, and the user interface is done
using Qt Quick and Qt Quick Components. Besides the socket communication, this
project also demonstrates UI and game logic separation, utilisation of C++
models in QML, and exposing enumerations, defined in C++ classes, to QML.

This example application is hosted in Nokia Developer Projects:
- http://projects.developer.nokia.com/tictactoe

For more information on implementation and porting, visit the wiki pages:
- http://projects.developer.nokia.com/tictactoe/wiki
- http://projects.developer.nokia.com/tictactoe/wiki/porting

The list of Windows Phone examples including the Tic-Tac-Toe Over Sockets Sample
can be found from the MSDN Library:
- http://msdn.microsoft.com/en-us/library/ff431744.aspx

What's new in version 1.1
-------------------------

 * The connection management of WLAN connectivity was extracted into its own
   class, ConnectionManager, which is designed to be easily separable. It has no
   dependencies to any other classes in the application. The new class is owned
   and used by the TicTacServer class.

 * The access point can be changed without restarting the application.

 * 'Connection error' error message was added to the UI.
 
 * A bug between a connection of two Harmattan devices was fixed.


1. How to play
-------------------------------------------------------------------------------

To play the game you need two devices with the game installed. To
establish a connection to another device, make sure that the devices are
connected to the same wireless network, and then launch the application. The
devices will establish the connection automatically when the application is
launched. After the game has finished, tap the refresh button in the toolbar to
start a new game.


2. Project structure
-------------------------------------------------------------------------------

2.1 Folders
-----------

 |                  The root folder contains the project file, resource files,
 |                  the license information and this file (release notes).
 |
 |- gfx             Contains application graphics.
 |
 |- icons           Contains application icons.
 |
 |- qml             Root folder for QML and Javascript files.
 |  |
 |  |- common       Common, cross-platform QML files.
 |  |
 |  |- harmattan    Harmattan-specific QML files.
 |  |
 |  |- symbian      Symbian-specific QML files.
 |
 |- qtc_packaging   Contains the Harmattan (Debian) packaging files.
 |
 |- src             Contains the Qt/C++ source code files.


2.2 Important files
-------------------

| File                           | Description                                 |
|--------------------------------|---------------------------------------------|
| qml/common/TicTac.qml          | Represents a single grid item on the game   |
|                                | board.                                      |
|--------------------------------|---------------------------------------------|
| qml/common/TicTacGrid.qml      | Represents the game board.                  |
|--------------------------------|---------------------------------------------|
| qml/<platform>/GamePage.qml    | The main view of the application.           |
|--------------------------------|---------------------------------------------|
| qml/<platform>/main.qml        | The main QML file containing the            |
|                                | application window properties, page stack   |
|                                | etc.                                        |
|--------------------------------|---------------------------------------------|
| qml/<platform>/StatusLabel.qml | Label for displaying the game state.        |
|--------------------------------|---------------------------------------------|
| src/connectionmanager.*        | ConnectionManager class implementation.     |
|                                | This class is responsible for managing the  |
|                                | WLAN connections.                           |
|--------------------------------|---------------------------------------------|
| src/gamecommunicator.h         | GameCommunicator class declaration and      |
|                                | partial implementation. Base class for      |
|                                | client and server classes.                  |
|--------------------------------|---------------------------------------------|
| src/gamemodel.*                | GameModel class implementation.             |
|                                | Contains the game data which is visualised  |
|                                | by the QML user interface.                  |
|--------------------------------|---------------------------------------------|
| src/loadhelper.h               | LoadHelper class declaration.               |
|                                | The methods of the class are implemented in |
|                                | main.cpp. This utility class is used to     |
|                                | load the main UI after the splash screen    |
|                                | has been shown.                             |
|--------------------------------|---------------------------------------------|
| src/protocolutility.*          | ProtocolUtility class implementation.       |
|                                | Interprets and creates messages for         |
|                                | communication.                              |
|--------------------------------|---------------------------------------------|
| src/tictacclient.*             | TicTacClient class implementation.          |
|                                | The application client.                     |
|--------------------------------|---------------------------------------------|
| src/tictacengine.*             | TicTacEngine class implementation.          |
|                                | The game engine that runs the application   |
|                                | and owns the other class instances.         |
|--------------------------------|---------------------------------------------|
| src/tictacserver.*             | TicTacServer class implementation.          |
|                                | The application server.                     |
|--------------------------------|---------------------------------------------|


3. Compatibility
-------------------------------------------------------------------------------

 - Symbian devices with Qt 4.7.4, Qt Mobility 1.2.1 and Qt Quick Components 1.1
   or higher.
 - MeeGo 1.2 Harmattan.

Tested to work on Nokia C6-01, Nokia C7-00, Nokia X7-00, Nokia N9, and
Nokia N950. Developed with Qt SDK 1.1.

3.1 Required capabilities
-------------------------

None; The application can be self signed on Symbian.


3.2 Known issues
----------------

None.


4. Building, installing, and running the application
-------------------------------------------------------------------------------

4.1 Preparations
----------------

Check that you have the latest Qt SDK installed in the development environment
and the latest Qt version on the device.

Qt Quick Components 1.0 or higher required.

4.2 Using Qt SDK
----------------

You can install and run the application on the device by using the Qt SDK.
Open the project in the SDK, set up the correct target (depending on the device
platform), and click the Run button. For more details about this approach, 
visit the Qt Getting Started at Nokia Developer
(http://www.developer.nokia.com/Develop/Qt/Getting_started/).

4.3 Symbian device
------------------

Make sure your device is connected to your computer. Locate the .sis
installation file and open it with Ovi Suite. Accept all requests from Ovi
Suite and the device. Note that you can also install the application by copying
the installation file onto your device and opening it with the Symbian File
Manager application.

After the application is installed, locate the application icon from the
application menu and launch the application by tapping the icon.

4.4 Nokia N9 and Nokia N950
---------------------------

Copy the application Debian package onto the device. Locate the file with the
device and run it; this will install the application. Note that you can also
use the terminal application and install the application typing the command
'dpkg -i <package name>.deb' on the command line. To install the application
using the terminal application, make sure you have the right privileges 
to do so (e.g. root access).

Once the application is installed, locate the application icon from the
application menu and launch the application by tapping the icon.


5. License
-------------------------------------------------------------------------------

See the license text file delivered with this project. The license file is also
available online at
https://projects.developer.nokia.com/tictactoe/browser/trunk/Licence.txt


6. Version history
-------------------------------------------------------------------------------

1.1.1 Ad-hoc fix for Harmattan
1.1 Code refactoring and bug fixes
1.0 Initial release
