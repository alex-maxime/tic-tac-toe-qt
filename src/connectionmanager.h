/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QNetworkConfiguration>
#include <QNetworkSession>
#include <QObject>
#include <QString>

// Forward declarations
class QNetworkConfigurationManager;


class ConnectionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString accessPoint READ accessPoint NOTIFY accessPointChanged)
    Q_PROPERTY(QString myIp READ myIp NOTIFY myIpChanged)

public:
    enum ConnectionState {
        NotConnected = 0,
        Connecting,
        Connected,
        Disconnecting,
        Failed
    };

public:
    explicit ConnectionManager(QObject *parent = 0);
    virtual ~ConnectionManager();

public:
    int state() const;
    QString accessPoint() const;
    QString myIp() const;

public slots:
    void connectToWlan();

private:
    void setState(ConnectionState state);
    bool validWlanConfiguration(QNetworkConfiguration &configuration) const;
    void clearConnectionInformation();

private slots:
    void openNewNetworkSession();
    void disconnect();
    void handleStateChanged(QNetworkSession::State state);
    void handleNetworkSessionOpened();
    void handleNetworkSessionClosed();
    void handleNewConfigurationActivated();
    void handleError(QNetworkSession::SessionError error);

signals:
    void stateChanged(int state);
    void accessPointChanged(QString accessPoint);
    void myIpChanged(QString myIp);

private: // Data
    QNetworkSession *mNetworkSession; // Owned
    QNetworkConfigurationManager *mNetworkConfMgr; // Owned
    QString mAccessPoint;
    QHostAddress mMyIp;
    ConnectionState mState;
};


#endif // CONNECTIONMANAGER_H

// End of file.
