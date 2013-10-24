/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef TICTACSERVER_H
#define TICTACSERVER_H

#include <QObject>
#include <QTimer>

#include "gamecommunicator.h"

// Forward declarations
class ConnectionManager;
class QTcpServer;
class QUdpSocket;


class TicTacServer : public GameCommunicator
{
    Q_OBJECT
    Q_PROPERTY(int state READ state NOTIFY stateChanged)

public:
    enum ServerStatus {
        Stopped = 0,
        Starting,
        Broadcasting,
        ClientConnected,
        ConnectionError
    };

public:
    explicit TicTacServer(QObject *parent = 0);
    virtual ~TicTacServer();

public:
    int state() const;
    ConnectionManager *connectionManager() const;

public slots:
    void startServer();
    void stopServer();

private slots:
    void handleConnectionManagerStateChanged(int state);
    void establishConnection();
    void readIncomingMessage();
    void broadcastServerInfo();

private:
    void setState(int state);

signals:
    void stateChanged(int state);

private: // Data
    ConnectionManager *mConnectionManager; // Owned
    QTcpServer *mTcpServer; // Owned
    QUdpSocket *mBroadcastSocket; // Owned
    QTimer mBroadcastTimer;
    QHostAddress mPeerAddress;
    int mState;
};

#endif // TICTACSERVER_H
