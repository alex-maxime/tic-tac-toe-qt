/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include "connectionmanager.h"

#include <QDebug>
#include <QNetworkConfigurationManager>


/*!
  \class ConnectionManager
  \brief Manages the WIFI connectivity.
*/


/*!
  Constructor.
*/
ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent),
      mNetworkSession(0),
      mNetworkConfMgr(0),
      mState(NotConnected)
{
    // Initialize own IP to localhost.
    mMyIp.setAddress(QHostAddress::LocalHost);

    mNetworkConfMgr = new QNetworkConfigurationManager(this);

    connect(mNetworkConfMgr, SIGNAL(updateCompleted()),
            this, SLOT(openNewNetworkSession()));
}


/*!
  Destructor.
*/
ConnectionManager::~ConnectionManager()
{
    qDebug() << "ConnectionManager::~ConnectionManager(): =>";
    disconnect();
    delete mNetworkSession;
    mNetworkSession = 0;
    setState(NotConnected);
    qDebug() << "ConnectionManager::~ConnectionManager(): <=";
}


/*!
  Returns the state of the connection manager.
*/
int ConnectionManager::state() const
{
    return (int)mState;
}


/*!
  Returns the access point of the current connection.
*/
QString ConnectionManager::accessPoint() const
{
    return mAccessPoint;
}


/*!
  Returns the IP of the current connection.
*/
QString ConnectionManager::myIp() const
{
    return mMyIp.toString();
}


/*!
  Disconnects (if connected) and updates the network configurations. Once the
  update is complete, the manager will try to automatically connect to a
  network.
*/
void ConnectionManager::connectToWlan()
{
    qDebug() << "ConnectionManager::connectToWlan()";
    disconnect();
    mNetworkConfMgr->updateConfigurations();
}


/*!
  Sets the state of the connection manager to \a state.
*/
void ConnectionManager::setState(ConnectionState state)
{
    if (state != mState) {
        mState = state;
        emit stateChanged((int)mState);
    }
}


/*!
  Returns true if \a configuration is a valid WLAN network configuration.
  False otherwise.
*/
bool ConnectionManager::validWlanConfiguration(
        QNetworkConfiguration &configuration) const
{
    bool valid =
#ifdef Q_OS_SYMBIAN
            (configuration.isValid() &&
             configuration.bearerType() == QNetworkConfiguration::BearerWLAN);
#else
            (configuration.isValid() &&
             (configuration.bearerType() == QNetworkConfiguration::BearerWLAN
              || configuration.type() == QNetworkConfiguration::ServiceNetwork));
#endif

    if (valid) {
        qDebug() << "ConnectionManager::validWlanConfiguration():"
                 << configuration.name()
                 << configuration.bearerTypeName()
                 << configuration.bearerType()
                 << configuration.type()
                 << configuration.state()
                 << configuration.isValid();
    }

    return valid;
}


/*!
  Opens a new network session.
*/
void ConnectionManager::openNewNetworkSession()
{
    qDebug() << "ConnectionManager::openNewNetworkSession() =>";

    if (mState == Connecting) {
        qDebug() << "ConnectionManager::openNewNetworkSession()"
                 << "<= Already connecting!!!";
        return;
    }

    // Disconnect previous connection and delete the network session instance.
    disconnect();
    clearConnectionInformation();
    delete mNetworkSession;
    mNetworkSession = 0;

    // Set state to 'Connecting'.
    setState(Connecting);

    // Get the configurations.
    QList<QNetworkConfiguration> configurations =
            mNetworkConfMgr->allConfigurations(QNetworkConfiguration::Discovered);

    if (configurations.isEmpty()) {
        configurations << mNetworkConfMgr->defaultConfiguration();
    }

    bool sessionOpened(false);

    foreach (QNetworkConfiguration configuration, configurations) {
        if (!validWlanConfiguration(configuration)) {
            // Invalid configuration, try the next one.
            continue;
        }

        // Valid configuration found!
        mNetworkSession = new QNetworkSession(configuration, this);

        // Open the network session.
        mNetworkSession->open();

        if (mNetworkSession->waitForOpened()) {
            qDebug() << "ConnectionManager::openNewNetworkSession():"
                     << "Selecting" << configuration.name();

            // Connect the signals.
            connect(mNetworkSession, SIGNAL(closed()),
                    this, SLOT(handleNetworkSessionClosed()),
                    Qt::UniqueConnection);
            connect(mNetworkSession, SIGNAL(error(QNetworkSession::SessionError)),
                    this, SLOT(handleError(QNetworkSession::SessionError)),
                    Qt::UniqueConnection);
            connect(mNetworkSession, SIGNAL(newConfigurationActivated()),
                    this, SLOT(handleNewConfigurationActivated()),
                    Qt::UniqueConnection);
            connect(mNetworkSession, SIGNAL(stateChanged(QNetworkSession::State)),
                    this, SLOT(handleStateChanged(QNetworkSession::State)),
                    Qt::UniqueConnection);

            handleNetworkSessionOpened();
            sessionOpened = true;

            break;
        }
        else {
            qDebug() << "ConnectionManager::openNewNetworkSession():"
                     << "Failed to open" << configuration.name()
                     << ":" << mNetworkSession->errorString();

            delete mNetworkSession;
            mNetworkSession = 0;
        }
    }

    if (!sessionOpened) {
        qDebug() << "ConnectionManager::openNewNetworkSession():"
                 << "No valid session opened!";
        setState(Failed);
    }

    qDebug() << "ConnectionManager::openNewNetworkSession() <=";
}


/*!
  Disconnects if connected.
*/
void ConnectionManager::disconnect()
{
    if (mNetworkSession && mNetworkSession->isOpen()) {
        qDebug() << "ConnectionManager::disconnect(): Disconnecting...";
        mNetworkSession->close();
        setState(Disconnecting);
    }
}


/*!
  Handles the changed \a state of the network session.
*/
void ConnectionManager::handleStateChanged(QNetworkSession::State state)
{
    qDebug() << "ConnectionManager::handleStateChanged():" << state;

    switch (state) {
    case QNetworkSession::Invalid: {
        clearConnectionInformation();
        setState(NotConnected);
        break;
    }
    case QNetworkSession::NotAvailable: {
        clearConnectionInformation();
        setState(NotConnected);
        break;
    }
    case QNetworkSession::Connecting: {
        break;
    }
    case QNetworkSession::Connected: {
        break;
    }
    case QNetworkSession::Closing: {
        setState(Disconnecting);
        break;
    }
    case QNetworkSession::Disconnected: {
        clearConnectionInformation();
        setState(NotConnected);
        break;
    }
    case QNetworkSession::Roaming: {
        break;
    }
    } // switch (state)
}


/*!
  Checks the configuration and updates the connection information if the
  connection is valid.
*/
void ConnectionManager::handleNetworkSessionOpened()
{
    // Go through all interfaces and add all IPs for interfaces that can
    // broadcast and are not local.
    foreach (QNetworkInterface networkInterface,
             QNetworkInterface::allInterfaces())
    {
        if (!networkInterface.addressEntries().isEmpty()
                && networkInterface.isValid())
        {
            QNetworkInterface::InterfaceFlags interfaceFlags =
                    networkInterface.flags();

            if (interfaceFlags & QNetworkInterface::IsUp
                    && interfaceFlags & QNetworkInterface::IsRunning
                    && interfaceFlags & QNetworkInterface::CanBroadcast
                    && !(interfaceFlags & QNetworkInterface::IsLoopBack))
            {
                qDebug() << "ConnectionManager::handleNetworkSessionOpened():"
                         << "The connection is valid!";

                if (mNetworkSession) {
                    mAccessPoint = mNetworkSession->configuration().name();
                    emit accessPointChanged(mAccessPoint);
                }

                mMyIp.setAddress(networkInterface.addressEntries().at(0).ip().toString());
                emit myIpChanged(mMyIp.toString());

                setState(Connected);
                return;
            }
        }
    }

    qDebug() << "ConnectionManager::handleNetworkSessionOpened():"
             << "The connection was not valid!";
    setState(Failed);
}


/*!
  Updates the state to 'NotConnected' and deletes the network session instance.
*/
void ConnectionManager::handleNetworkSessionClosed()
{
    qDebug() << "ConnectionManager::handleNetworkSessionClosed(): =>";
    clearConnectionInformation();
    setState(NotConnected);
    qDebug() << "ConnectionManager::handleNetworkSessionClosed(): <=";
}


/*!
  Updates the network configurations.
*/
void ConnectionManager::handleNewConfigurationActivated()
{
    qDebug() << "ConnectionManager::handleNewConfigurationActivated()";
    connectToWlan();
}


/*!
  Handles \a error of the network session.
*/
void ConnectionManager::handleError(QNetworkSession::SessionError error)
{
    qDebug() << "ConnectionManager::handleError():" << error;

    if (mNetworkSession) {
        qDebug() << "ConnectionManager::handleError():"
                 << mNetworkSession->errorString();
    }

    switch (error) {
    case QNetworkSession::UnknownSessionError: {
        break;
    }
    case QNetworkSession::SessionAbortedError: {
        break;
    }
    case QNetworkSession::RoamingError: {
        break;
    }
    case QNetworkSession::OperationNotSupportedError: {
        break;
    }
    case QNetworkSession::InvalidConfigurationError: {
        break;
    }
    }

    clearConnectionInformation();
    setState(Failed);
}


/*!
  Clears the connection information.
*/
void ConnectionManager::clearConnectionInformation()
{
    qDebug() << "ConnectionManager::clearConnectionInformation()";

    // Clear the access point and initialize own IP to localhost.
    mAccessPoint = QString();
    mMyIp.setAddress(QHostAddress::LocalHost);
    emit accessPointChanged(mAccessPoint);
    emit myIpChanged(mMyIp.toString());
}


// End of file.
