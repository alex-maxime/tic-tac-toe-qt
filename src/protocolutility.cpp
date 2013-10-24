/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#include "protocolutility.h"
#include <QDebug>
#include <QStringList>


/*!
  \class ProtocolUtility
  \brief Utility class for interpreting communicated messages and encoding
         the local user moves to messages.
*/


/*!
  Constructor.
*/
ProtocolUtility::ProtocolUtility(QObject *parent) :
    QObject(parent)
{
}


/*!
  Utility function to convert the egiven \a mark to a string.
*/
QString ProtocolUtility::markToString(int mark)
{
    QString str = QString();

    if (mark == GameModel::Zero) {
        str = "O";
    }
    else if (mark == GameModel::Cross) {
        str = "X";
    }

    return str;
}


/*!
  Constructs and returns a message for communicating to client based on the
  given \a position.
*/
QByteArray ProtocolUtility::constructMessage(bool isClient,
                                             int position,
                                             const GameModel &board)
{
    QByteArray message;

    if (!isClient) {
        // A typical response: "tb_11",
        // where row 1, col 1 has been marked by the local user
        QString str("tb_"); // tb_rc, where r:row, c:col
        str.append(QString::number(position / ROW_COUNT)); // row
        str.append(QString::number(position % COLUMN_COUNT)); // col
        qDebug() << "constructMessage() <>" << str;
        message = str.toAscii();
    }
    else {
        char opponentMark = (board.data(board.index(position)) == GameModel::Zero)
                ? 'X'
                : 'O';
        QString msg(opponentMark);

        for (int i = 0; i < ROW_COUNT; i++) {
            for (int j = 0; j < COLUMN_COUNT; j++) {
                int value = board.data(board.index(i*COLUMN_COUNT + j)).toInt();
                msg += "|tb_"
                        + QString::number(i)
                        + QString::number(j)
                        + '*'
                        + markToString(value);
            }
        }

        qDebug() << "ProtocolUtility::constructMessage():" << msg.toAscii();
        message = msg.toAscii();
    }

    return message;
}


/*!
  Interpret's the \a message coming from the client.
  Obtins the \a changedPosition based on the \a previousState and \a message.
  Here's a typical message sent by the client:
  "O|tb_00*|tb_01*|tb_02*X|tb_10*|tb_11*|tb_12*|tb_20*|tb_21*|tb_22*<EOF>"
  where row 0, col 2 has an X mark.
*/
bool ProtocolUtility::interpretMessage(bool isClient,
                                       QString message,
                                       const GameModel &previousState,
                                       int &changedPosition,
                                       int &localUserMark)
{
    qDebug() << "ProtocolUtility::interpretMessage():" << message;
    bool messageOk(false);

    if (isClient) {
        QRegExp rx("tb_\\d{2}"); // tb_11

        if (0 == message.indexOf(rx)) {
            // Valid message
            changedPosition = ROW_COUNT * message.mid(3,1).toInt()
                              + message.mid(4,1).toInt();
            messageOk = true;
        }
    }
    else {
        QString str;
        QChar ch;
        int index(0);
        int pos(0);
        QRegExp rx("tb_\\d{2}\\*[XO]"); // Represents a marked location

        // The first character represents server side's mark
        localUserMark = (message[0] == QChar('O')) ? GameModel::Zero
                                                   : GameModel::Cross;

        // Go through all marked locations, until a changed location is found
        while ((index = message.indexOf(rx, index)) != -1) {
            str = message.mid(index + 3, 2); // rc, where r:row, c:col
            pos = ROW_COUNT * str.left(1).toInt() + str.right(1).toInt();
            ch = message.at(index + 6);

            // Compare with the previous state of the location to see if this
            // is the changed location.
            int prevVal = previousState.data(previousState.index(pos)).toInt();

            if (prevVal == GameModel::None && (ch == 'O' || ch == 'X' )) {
                changedPosition = pos;
                messageOk = true;
                break;
            }

            index++;
        }
    }

    return messageOk;
}
