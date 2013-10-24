/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef GAMECOMMUNICATOR_H
#define GAMECOMMUNICATOR_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QHostAddress>

const int SERVER_PORT = 13001;
const int BROADCAST_PORT = 13002;


/*!
  \class GameCommunicator
  \brief Base class for Client and Server classes.
*/
class GameCommunicator : public QObject
{
    Q_OBJECT

public:
    GameCommunicator(QObject *parent = NULL) : QObject(parent), mCommSocket(0) {}

public slots:

    /*!
      Sends game data in \a msg to the connected device.
    */
    void sendMessage(QByteArray msg)
    {
        if (mCommSocket && mCommSocket->isValid()) {
            mCommSocket->write(msg);
            mCommSocket->flush();
            qDebug() << "Message sent to" << mCommSocket->peerAddress().toString();
        }
    }

signals:
    void messageReceived(QString msg);

protected:
    QTcpSocket *mCommSocket;
};


#endif // GAMECOMMUNICATOR_H
