/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef TICTACENGINE_H
#define TICTACENGINE_H

#include <QObject>
#include "gamemodel.h"

// Forward declarations
class GameCommunicator;
class TicTacServer;
class TicTacClient;


class TicTacEngine : public QObject
{
    Q_OBJECT

    Q_ENUMS(GameStatus)
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(int userMark READ userMark NOTIFY userMarkChanged)
    Q_PROPERTY(QString ssid READ ssid NOTIFY ssidChanged)
    Q_PROPERTY(QString ipAndPort READ ipAndPort NOTIFY ipAndPortChanged)

public:
    enum GameStatus {
        None = 0,       // Not started.
        Started,        // Game has been started / waiting for connection.
        WaitUser,       // Waiting for local user.
        WaitOpponent,   // Wait for remote user / opponent.
        GameOver,       // Game over. Call winner() to determine the winner.
        ConnectionError
    };

public: // Constructor
    explicit TicTacEngine(QObject *parent = 0);

public:
    GameModel *gameData() { return &mGameModel; }
    int status() const { return mGameStatus; }
    int userMark() const { return mUserMark; }
    QString ssid() const { return mSSID; }
    QString ipAndPort() const { return mIpAndPort; }

public slots:
    void startGame();
    void terminateConnection();
    void messageReceived(QString);
    void mark(int location, int userMark = GameModel::Zero);
    bool isGameOver();
    int winner();

private slots:
    void handleServerState(int state);
    void updateConnectionInformation();
    void proceedAsClient();

private: // Utility functions
    void initGame();
    void setStatus(GameStatus status);
    bool isDraw();

signals:
    void statusChanged();
    void userMarkChanged(int);
    void ssidChanged(QString ssid);
    void ipAndPortChanged(QString ipAndPort);

private: // Data
    TicTacServer *mServer; // Owned
    TicTacClient *mClient; // Owned
    GameCommunicator *mActiveConn;
    GameModel mGameModel;
    int mGameStatus;
    int mUserMark;
    int mRemoteUserMark;
    bool mUserTurn;
    bool mIsClient;
    QString mSSID;
    QString mIpAndPort;
};

#endif // TICTACENGINE_H
