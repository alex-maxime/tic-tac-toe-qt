/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef PROTOCOLUTILITY_H
#define PROTOCOLUTILITY_H

#include <QObject>
#include "gamemodel.h"

class ProtocolUtility : public QObject
{
    Q_OBJECT

public:
    explicit ProtocolUtility(QObject *parent = 0);

public:
    static QString markToString(int mark);
    static QByteArray constructMessage(bool isClient,
                                       int position,
                                       const GameModel &board);
    static bool interpretMessage(bool isClient,
                                 QString message,
                                 const GameModel &previousState,
                                 int &changedPosition,
                                 int &localUserMark);
};

#endif // PROTOCOLUTILITY_H
