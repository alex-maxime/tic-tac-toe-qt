/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#include "tictacclient.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QUdpSocket>


/*!
  \class TicTacClient
  \brief Provides a client communication feature.
*/


/*!
  Constructor.
*/
TicTacClient::TicTacClient(QObject *parent) :
    GameCommunicator(parent),
    mDiscoverySocket(0)
{
    // Construct a socket to listen for broadcasted server info.
    mDiscoverySocket = new QUdpSocket(this);
}


/*!
  Read the incoming data and signal the data to listeners.
*/
void TicTacClient::readIncomingMessage()
{
    QString msg(mCommSocket->readAll());
    emit messageReceived(msg);
}


/*!
  Start listening for broadcasted TicTac server info.
*/
void TicTacClient::startDiscovery()
{
    qDebug() << "TicTacClient::startDiscovery():"
             << "Will stop discovery first to make sure there are no ongoing"
             << "connections before proceeding.";

    stopDiscovery();

    // Close previously opened communication socket if open
    if (mCommSocket) {
        mCommSocket->abort();
        mCommSocket->close();

        delete mCommSocket;
        mCommSocket = 0;
    }

    qDebug() << "TicTacClient::startDiscovery():"
             << "Starting to listen for server broadcast...";

    mDiscoverySocket->bind(QHostAddress::Any, BROADCAST_PORT);
    connect(mDiscoverySocket, SIGNAL(readyRead()),
            this, SLOT(readBroadcast()));
}


/*!
  Stop the discovery of server(s).
*/
void TicTacClient::stopDiscovery()
{
    qDebug() << "TicTacClient::stopDiscovery()";

    if (mDiscoverySocket) {
        mDiscoverySocket->close();
        /*qDebug() << "TicTacClient::stopDiscovery(): Discovery socket state is"
                 << mDiscoverySocket->state();*/
    }
}


/*!
  Read the broadcasted server info. If a remote game server is found,
  then connect to it and signal the server availability to engine.
*/
void TicTacClient::readBroadcast()
{
    while (!mCommSocket && mDiscoverySocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(mDiscoverySocket->pendingDatagramSize());
        mDiscoverySocket->readDatagram(datagram.data(), datagram.size());

        if (datagram.startsWith("TICTAC")) {
            QList<QByteArray> ls = datagram.mid(7).split(':');
            QHostAddress address = QHostAddress(QString(ls[0]));
            int port = ls.at(1).toInt();

            if (!QNetworkInterface::allAddresses().contains(address)) {
                // Create a socket for communicating with the server
                mCommSocket = new QTcpSocket(this);
                mCommSocket->connectToHost(address, port);

                connect(mCommSocket, SIGNAL(readyRead()),
                        this, SLOT(readIncomingMessage()));

                mDiscoverySocket->close();

                // Notify the engine
                qDebug() << "TicTacClient::readBroadcast(): Found another gamer:"
                         << qPrintable(address.toString()) << port;
                emit serverFound(address.toString(), port);
                break;
            }
        }
    }
}
