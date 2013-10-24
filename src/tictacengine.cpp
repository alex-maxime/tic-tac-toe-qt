/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#include "tictacengine.h"

#include <QDebug>
#include <QStringList>

#include "connectionmanager.h"
#include "gamecommunicator.h"
#include "protocolutility.h"
#include "tictacclient.h"
#include "tictacserver.h"


/*!
  \class TicTacEngine
  \brief Handles the logic of the game.
*/


/*!
  Constructor.
*/
TicTacEngine::TicTacEngine(QObject *parent) :
    QObject(parent),
    mServer(0),
    mClient(0),
    mActiveConn(0),
    mGameStatus(None),
    mUserMark(GameModel::None),
    mRemoteUserMark(GameModel::None),
    mUserTurn(false),
    mIsClient(false)
{
    // Construct the server and the client and connect signals.
    mServer = new TicTacServer(this);

    connect(mServer, SIGNAL(stateChanged(int)),
            this, SLOT(handleServerState(int)));

    ConnectionManager *connectionManager = mServer->connectionManager();

    connect(connectionManager, SIGNAL(myIpChanged(QString)),
            this, SLOT(updateConnectionInformation()));

    mClient = new TicTacClient(this);

    connect(mClient, SIGNAL(serverFound(QString,int)),
            this, SLOT(proceedAsClient()));
}


/*!
  Starts a new game.
*/
void TicTacEngine::startGame()
{
    qDebug() << "TicTacEngine::startGame()";

    setStatus(None);
    initGame();

    // Start game as server by default
    mServer->startServer();
    mClient->startDiscovery();
    mActiveConn = mServer;
    setStatus(Started);
}


/*!
  Terminates all connection activities. This slot can be called e.g. when the
  searching of an opponent timeouts.
*/
void TicTacEngine::terminateConnection()
{
    qDebug() << "TicTacEngine::terminateConnection()";
    mServer->stopServer();
    mClient->stopDiscovery();
    mActiveConn = 0;
}


/*!
  Slot for handling received \a message from opponent.
*/
void TicTacEngine::messageReceived(QString message)
{
    qDebug() << "TicTacEngine::messageReceived():" << message;

    int changedPosition(-1);
    int localUserMark(GameModel::None);

    if (!mUserTurn && ProtocolUtility::interpretMessage(
                mIsClient, message, mGameModel, changedPosition, localUserMark)) {
        // The marks are decided when the first turn is done by client
        if (mUserMark == GameModel::None) {
            qDebug() << "TicTacEngine::messageReceived(): User mark changed to" << mUserMark;
            mUserMark = localUserMark;
            mRemoteUserMark = (localUserMark == GameModel::Zero) ?
                        GameModel::Cross : GameModel::Zero;
            emit userMarkChanged(mUserMark);
        }

        mGameModel.setData(changedPosition, mRemoteUserMark);

        if (!isGameOver()) {
            mUserTurn = true;
            setStatus(WaitUser);
        }
    } else {
        qDebug() << "TicTacEngine::messageReceived(): Invalid message or position!";
    }
}


/*!
  Slot to make local user's mark on the given \a location and send to the
  other side.
*/
void TicTacEngine::mark(int location, int userMark)
{
    qDebug("TicTacEngine::mark(%d, %d)", location, userMark);

    if (mUserTurn) {
        if (mUserMark == GameModel::None) {
            mUserMark = userMark;
            mRemoteUserMark = (userMark == GameModel::Zero) ? GameModel::Cross : GameModel::Zero;
            emit userMarkChanged(mUserMark);
        }

        if (mGameModel.data(mGameModel.index(location)) == GameModel::None) {
            mGameModel.setData(location, mUserMark);
            mActiveConn->sendMessage(ProtocolUtility::constructMessage(
                                     mIsClient, location, mGameModel));

            if (!isGameOver()) {
                mUserTurn = false;
                setStatus(WaitOpponent);
            }
        }
    }
    else {
        if (status() != Started && status() != WaitOpponent && !isGameOver()) {
            setStatus(WaitOpponent);
        }
    }
}


/*!
  Slot to check if the current game is over, returns boolean value.
*/
bool TicTacEngine::isGameOver()
{
    bool gameOver(false);

    int theWinner = winner();

    if (theWinner != GameModel::None) {
        qDebug() << "TicTacEngine::isGameOver(): Yes!";
        gameOver = true;
        setStatus(GameOver);
    }
    else if (isDraw()) {
        gameOver = true;
        setStatus(GameOver);
    }

    return gameOver;
}


/*!
  Checks if one of the users has won the game and returns the winner's TicTac
  mark.
*/
int TicTacEngine::winner()
{
    // Make a copy of the model data
    int i, j;
    int winningArray[ROW_COUNT] = {0, 0, 0};
    bool combo = false;
    GameModel::TicTacType modelCopy[ROW_COUNT][COLUMN_COUNT];

    for (i = 0; i < ROW_COUNT; i++) {
        for (j = 0; j < COLUMN_COUNT; j++) {
            modelCopy[i][j] = (GameModel::TicTacType)(mGameModel.data(
                            mGameModel.index(i * ROW_COUNT + j, 0)).toInt());
        }
    }

    // Horizontal combinations
    for (i = 0; !combo && i < ROW_COUNT; i++) {
        if (modelCopy[i][0] != GameModel::None) {
            combo = true;
            winningArray[0] = i * ROW_COUNT;

            for (j = 1; j < COLUMN_COUNT; j++) {
                if (modelCopy[i][0] == modelCopy[i][j]) {
                    winningArray[j] = i * ROW_COUNT + j;
                }
                else {
                    combo = false;
                    break;
                }
            }
        }
    }

    // Vertical combinations
    for (i = 0, j = 0; !combo && j < COLUMN_COUNT; j++) {
        if (modelCopy[0][j] != GameModel::None) {
            combo = true;
            winningArray[0] = j;

            for (i = 1; i < ROW_COUNT; i++) {
                if (modelCopy[0][j] == modelCopy[i][j]) {
                    winningArray[i] = i * ROW_COUNT + j;
                }
                else {
                    combo = false;
                    break;
                }
            }
        }
    }

    // Leading diagonal
    if (!combo && modelCopy[0][0] != GameModel::None) {
        combo = true;
        winningArray[0] = 0;

        for (i = 1, j = 1; i < ROW_COUNT; i++, j++) {
            if (modelCopy[0][0] == modelCopy[i][j]) {
                winningArray[j] = i * ROW_COUNT + j;
            }
            else {
                combo = false;
                break;
            }
        }
    }

    // Other diagonal
    if (!combo && modelCopy[0][COLUMN_COUNT-1] != GameModel::None) {
        combo = true;
        winningArray[0] = COLUMN_COUNT - 1;

        for (i = 1, j = COLUMN_COUNT - 2; j >= 0; i++, j--) {
            if (modelCopy[0][COLUMN_COUNT - 1] == modelCopy[i][j]) {
                winningArray[i] = i * ROW_COUNT + j;
            }
            else {
                combo = false;
                break;
            }
        }
    }

    if (combo) {
        for (i = 0; i < ROW_COUNT; i++) {
            mGameModel.setData(winningArray[i], true, GameModel::HighlightRole);
        }

        return mGameModel.data(mGameModel.index(winningArray[0], 0)).toInt();
    }

    return GameModel::None;
}


/*!
  This slot is called when the state of the server has been changed to \a state.
*/
void TicTacEngine::handleServerState(int state)
{
    qDebug() << "TicTacEngine::handleServerState():" << state;

    switch (state) {
    case TicTacServer::Stopped: {
        mActiveConn = 0;
        break;
    }
    case TicTacServer::Starting: {
        break;
    }
    case TicTacServer::Broadcasting: {
        break;
    }
    case TicTacServer::ClientConnected: {
        if (status() == Started) {
            qDebug() << "TicTacEngine::handleServerState():"
                     << "Proceeding as server. Waiting for the move of the client...";

            // As we are the server, our client can now stop.
            mClient->stopDiscovery();

            mIsClient = false;
            mUserTurn = false;
            setStatus(WaitOpponent);

            // Register server as the active connection.
            mActiveConn = mServer;
            disconnect(this, SLOT(messageReceived(QString)));
            connect(mServer, SIGNAL(messageReceived(QString)),
                    this, SLOT(messageReceived(QString)));
        }

        break;
    }
    case TicTacServer::ConnectionError: {
        mActiveConn = 0;
        setStatus(ConnectionError);
        break;
    }
    default: {
        qDebug() << "TicTacEngine::handleServerState(): Error: Unknown state!";
    }
    }
}


/*!
  Updates the connection information and notifies the UI of the changes.
*/
void TicTacEngine::updateConnectionInformation()
{
    ConnectionManager *connectionManager = mServer->connectionManager();

    if (connectionManager) {
        mSSID = connectionManager->accessPoint();
        mIpAndPort = connectionManager->myIp() + ":" + QString::number(SERVER_PORT);
        emit ssidChanged(mSSID);
        emit ipAndPortChanged(mIpAndPort);
    }
}


/*!
  Slot to proceed the game as client. Local user can decide the marker(X/O)
  to use.
*/
void TicTacEngine::proceedAsClient()
{
    qDebug() << "TicTacEngine::proceedAsClient()";

    if (status() == Started) {
        // As we are the client, our server can now stop.
        mServer->stopServer();

        mIsClient = true;
        mUserTurn = true;
        setStatus(WaitUser);

        // Register client as the active connection.
        mActiveConn = mClient;
        disconnect(this, SLOT(messageReceived(QString)));
        connect(mClient, SIGNAL(messageReceived(QString)),
                this, SLOT(messageReceived(QString)));
    }
}


/*!
  Utility method to initialize the game data.
*/
void TicTacEngine::initGame()
{
    qDebug() << "TicTacEngine::initGame()";
    mGameModel.resetData();
    mUserMark = GameModel::None;
    mRemoteUserMark = GameModel::None;
    mUserTurn = false;
    emit userMarkChanged(mUserMark);
}


/*!
  Update the message property, to show the \a message to the user.
*/
void TicTacEngine::setStatus(GameStatus status)
{
    if (mGameStatus != status) {
        mGameStatus = status;
        emit statusChanged();
    }
}


/*!
  Checks if the game is a draw.
*/
bool TicTacEngine::isDraw()
{
    for (int i = 0; i < GRID_COUNT; ++i) {
        if (mGameModel.data(mGameModel.index(i, 0)) == GameModel::None) {
            return false;
        }
    }

    qDebug() << "TicTacEngine::isDraw(): Returning true!";
    return true;
}
