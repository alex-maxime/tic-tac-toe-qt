/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef TICTACCLIENT_H
#define TICTACCLIENT_H

#include <QObject>
#include "gamecommunicator.h"

class QTcpSocket;
class QUdpSocket;


class TicTacClient : public GameCommunicator
{
    Q_OBJECT

public:
    explicit TicTacClient(QObject *parent = 0);

public slots:
    void startDiscovery();
    void stopDiscovery();

private slots:
    void readBroadcast();
    void readIncomingMessage();

signals:
    void serverFound(QString ip, int port);

private:
    QUdpSocket *mDiscoverySocket;
};

#endif // TICTACCLIENT_H
