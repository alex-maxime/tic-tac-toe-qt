/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include "tictacserver.h"

#include <QDebug>
#include <QtNetwork>

#include "connectionmanager.h"

// Constants
const int BroadcastInterval(10000); // In milliseconds


/*!
  \class TicTacServer
  \brief Acts as a server for the TicTacToe game.
*/


/*!
  Constructor.
*/
TicTacServer::TicTacServer(QObject *parent):
    GameCommunicator(parent),
    mConnectionManager(0),
    mTcpServer(0),
    mBroadcastSocket(0),
    mPeerAddress(QHostAddress::Null),
    mState(Stopped)
{
    mConnectionManager = new ConnectionManager(this);
    connect(mConnectionManager, SIGNAL(stateChanged(int)),
            this, SLOT(handleConnectionManagerStateChanged(int)));
    mConnectionManager->connectToWlan();

    // Setup the timer for broadcasting the server info
    mBroadcastTimer.setInterval(BroadcastInterval);
    mBroadcastTimer.setSingleShot(false);
    connect(&mBroadcastTimer, SIGNAL(timeout()),
            this, SLOT(broadcastServerInfo()));
}


/*!
  Destructor.
*/
TicTacServer::~TicTacServer()
{
    qDebug() << "TicTacServer::~TicTacServer() =>";
    stopServer();
    qDebug() << "TicTacServer::~TicTacServer() <=";
}


/*!
  Returns the server state.
*/
int TicTacServer::state() const
{
    return mState;
}


/*!
  Returns the connection manager instance.
*/
ConnectionManager *TicTacServer::connectionManager() const
{
    return mConnectionManager;
}


/*!
  Start listening for incoming connections for the game.
*/
void TicTacServer::startServer()
{
    qDebug() << "TicTacServer::startServer()";

    if (mTcpServer && mTcpServer->isListening()) {
        qDebug() << "TicTacServer::startServer(): Will stop server.";
        stopServer();
    }

    if (mConnectionManager->state() == ConnectionManager::Connected) {
        // Server for listening to incoming game clients.
        mTcpServer = new QTcpServer(this);

        if (!mTcpServer->isListening()) {
            if (mTcpServer->listen(QHostAddress::Any, SERVER_PORT)) {
                connect(mTcpServer, SIGNAL(newConnection()),
                        this, SLOT(establishConnection()),
                        Qt::UniqueConnection);

                qDebug() << "TicTacServer::startServer(): Server listening on port"
                         << mTcpServer->serverPort();

                // Start broadcasting server IP periodically to invite clients
                if (!mBroadcastSocket) {
                    mBroadcastSocket = new QUdpSocket(this);
                }

                broadcastServerInfo();
                setState(Broadcasting);
            }
            else {
                qDebug() << "TicTacServer::startServer(): Server could not be started!";
            }
        }
    }
    else {
        qDebug() << "TicTacServer::startServer(): Pending start...";
        mConnectionManager->connectToWlan();
        setState(Starting);
    }
}


/*!
  Stops the server. Normally used when closing the application.
*/
void TicTacServer::stopServer()
{
    qDebug() << "TicTacServer::stopServer()";

    if (mTcpServer) {
        mTcpServer->close();
        delete mTcpServer;
        mTcpServer = 0;
        mCommSocket = 0; // Socket owned by server
    }

    // Stop broadcasting. Close the broadcast socket and clear the peer address.
    mBroadcastTimer.stop();

    if (mBroadcastSocket) {
        mBroadcastSocket->close();
        delete mBroadcastSocket;
        mBroadcastSocket = 0;
    }

    mPeerAddress = QHostAddress::Null;

    setState(Stopped);
}


/*!
  Handles the changed \a state of the connection manager.
*/
void TicTacServer::handleConnectionManagerStateChanged(int state)
{
    qDebug() << "TicTacServer::handleConnectionManagerStateChanged():" << state;

    switch (state) {
    case ConnectionManager::NotConnected: {
        stopServer();
        break;
    }
    case ConnectionManager::Connecting: {
        break;
    }
    case ConnectionManager::Connected: {
        // Start server, in case of pending start call
        if (mState == Stopped || mState == Starting) {
            startServer();
        }

        break;
    }
    case ConnectionManager::Disconnecting: {
        break;
    }
    case ConnectionManager::Failed: {
        mBroadcastTimer.stop();
        setState(ConnectionError);
        break;
    }
    }
}


/*!
  Slot to establish pending incoming connection. Skips incoming connections
  from different host during a game session.
*/
void TicTacServer::establishConnection()
{
    qDebug() << "TicTacServer::establishConnection()";

    QTcpSocket *socket = mTcpServer->nextPendingConnection();

    if (!mCommSocket || (socket && socket->peerAddress() == mPeerAddress)) {
        // Make a copy of the socket and connect to readyRead() signal
        mCommSocket = socket;
        connect(mCommSocket, SIGNAL(readyRead()),
                this, SLOT(readIncomingMessage()));

        if (mPeerAddress == QHostAddress::Null) {
            // New connection!
            // Stop broadcasting
            mBroadcastTimer.stop();

            if (mBroadcastSocket) {
                mBroadcastSocket->close();
                delete mBroadcastSocket;
                mBroadcastSocket = 0;
            }

            // Notify
            setState(ClientConnected);
        }

        // Make a copy of peer address to avoid incoming connections from any other host.
        mPeerAddress = mCommSocket->peerAddress();

        qDebug() << "TicTacServer::establishConnection(): Peer address:"
                 << mPeerAddress.toString() << ":" << mCommSocket->peerPort();
    }
    else {
        qDebug() << "TicTacServer::establishConnection():"
                 << "No pending connections or invalid connection:"
                 << "Error:" << mTcpServer->serverError()
                 << "Connection:"
                 << ((socket != 0) ? socket->peerAddress().toString() : "None");
    }
}


/*!
  Read the incoming data and post the data to listeners.
*/
void TicTacServer::readIncomingMessage()
{
    QString msg(mCommSocket->readAll());
    emit messageReceived(msg);
}


/*!
  Broadcast the availability of the Tic-Tac-Toe game server.
*/
void TicTacServer::broadcastServerInfo()
{
    if (mBroadcastSocket && mPeerAddress == QHostAddress::Null) {
        QString bdMsg = QString("TICTAC %1:%2").arg(mConnectionManager->myIp()).arg(SERVER_PORT);
        qDebug() << "TicTacServer::broadcastServerInfo():" << bdMsg;

#ifndef Q_OS_SYMBIAN
        // On Harmattan/Linux based OSes IP stack sends frames only to the
        // first network adapter if packets are sent to 255.255.255.255 or
        // QHostAddress::Broadcast on AUTOIP net
        QRegExp rx("169\.254\.\\d{1,3}.\\d{1,3}");

        if (rx.exactMatch(mConnectionManager->myIp())) {
            mBroadcastSocket->writeDatagram(bdMsg.toAscii(),
                                            QHostAddress("169.254.255.255"),
                                            BROADCAST_PORT);
        }
        else {
            mBroadcastSocket->writeDatagram(bdMsg.toAscii(),
                                            QHostAddress::Broadcast,
                                            BROADCAST_PORT);
        }
#else
        mBroadcastSocket->writeDatagram(bdMsg.toAscii(),
                                        QHostAddress::Broadcast,
                                        BROADCAST_PORT);
#endif

        mBroadcastSocket->flush();

        if (!mBroadcastTimer.isActive()) {
            // Re-broadcast until a client starts interacting.
            mBroadcastTimer.start();
        }
    }
}


/*!
  Sets the server state to \a state and notifies about the changed state by
  emitting stateChanged() signal.
*/
void TicTacServer::setState(int state)
{
    qDebug() << "TicTacServer::setState():" << state;
    mState = state;
    emit stateChanged(mState);
}
