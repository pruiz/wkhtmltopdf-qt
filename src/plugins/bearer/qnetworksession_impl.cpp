/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qnetworksession_impl.h"
#include "qbearerengine_impl.h"

#include <QtNetwork/qnetworksession.h>
#include <QtNetwork/private/qnetworkconfigmanager_p.h>

#include <QtCore/qstringlist.h>
#include <QtCore/qdebug.h>
#include <QtCore/qmutex.h>

#ifndef QT_NO_BEARERMANAGEMENT

QT_BEGIN_NAMESPACE

static QBearerEngineImpl *getEngineFromId(const QString &id)
{
    QNetworkConfigurationManagerPrivate *priv = qNetworkConfigurationManagerPrivate();

    foreach (QBearerEngine *engine, priv->engines()) {
        QBearerEngineImpl *engineImpl = qobject_cast<QBearerEngineImpl *>(engine);
        if (engineImpl && engineImpl->hasIdentifier(id))
            return engineImpl;
    }

    return 0;
}

class QNetworkSessionManagerPrivate : public QObject
{
    Q_OBJECT

public:
    QNetworkSessionManagerPrivate(QObject *parent = 0);
    ~QNetworkSessionManagerPrivate();

    void forceSessionClose(const QNetworkConfiguration &config);

Q_SIGNALS:
    void forcedSessionClose(const QNetworkConfiguration &config);
};

#include "qnetworksession_impl.moc"

Q_GLOBAL_STATIC(QNetworkSessionManagerPrivate, sessionManager);

QNetworkSessionManagerPrivate::QNetworkSessionManagerPrivate(QObject *parent)
:   QObject(parent)
{
}

QNetworkSessionManagerPrivate::~QNetworkSessionManagerPrivate()
{
}

void QNetworkSessionManagerPrivate::forceSessionClose(const QNetworkConfiguration &config)
{
    emit forcedSessionClose(config);
}

void QNetworkSessionPrivateImpl::syncStateWithInterface()
{
    connect(sessionManager(), SIGNAL(forcedSessionClose(QNetworkConfiguration)),
            this, SLOT(forcedSessionClose(QNetworkConfiguration)));

    opened = false;
    isOpen = false;
    state = QNetworkSession::Invalid;
    lastError = QNetworkSession::UnknownSessionError;

    qRegisterMetaType<QBearerEngineImpl::ConnectionError>
        ("QBearerEngineImpl::ConnectionError");

    switch (publicConfig.type()) {
    case QNetworkConfiguration::InternetAccessPoint:
        activeConfig = publicConfig;
        engine = getEngineFromId(activeConfig.identifier());
        if (engine) {
            qRegisterMetaType<QNetworkConfigurationPrivatePointer>("QNetworkConfigurationPrivatePointer");
            connect(engine, SIGNAL(configurationChanged(QNetworkConfigurationPrivatePointer)),
                    this, SLOT(configurationChanged(QNetworkConfigurationPrivatePointer)),
                    Qt::QueuedConnection);
            connect(engine, SIGNAL(connectionError(QString,QBearerEngineImpl::ConnectionError)),
                    this, SLOT(connectionError(QString,QBearerEngineImpl::ConnectionError)),
                    Qt::QueuedConnection);
        }
        break;
    case QNetworkConfiguration::ServiceNetwork:
        serviceConfig = publicConfig;
        // Defer setting engine and signals until open().
        // fall through
    case QNetworkConfiguration::UserChoice:
        // Defer setting serviceConfig and activeConfig until open().
        // fall through
    default:
        engine = 0;
    }

    networkConfigurationsChanged();
}

void QNetworkSessionPrivateImpl::open()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit QNetworkSessionPrivate::error(lastError);
    } else if (!isOpen) {
        if ((activeConfig.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            lastError =QNetworkSession::InvalidConfigurationError;
            state = QNetworkSession::Invalid;
            emit stateChanged(state);
            emit QNetworkSessionPrivate::error(lastError);
            return;
        }
        opened = true;

        if ((activeConfig.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active &&
            (activeConfig.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
            state = QNetworkSession::Connecting;
            emit stateChanged(state);

            engine->connectToId(activeConfig.identifier());
        }

        isOpen = (activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active;
        if (isOpen)
            emit quitPendingWaitsForOpened();
    }
}

void QNetworkSessionPrivateImpl::close()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit QNetworkSessionPrivate::error(lastError);
    } else if (isOpen) {
        opened = false;
        isOpen = false;
        emit closed();
    }
}

void QNetworkSessionPrivateImpl::stop()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit QNetworkSessionPrivate::error(lastError);
    } else {
        if ((activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            state = QNetworkSession::Closing;
            emit stateChanged(state);

            engine->disconnectFromId(activeConfig.identifier());

            sessionManager()->forceSessionClose(activeConfig);
        }

        opened = false;
        isOpen = false;
        emit closed();
    }
}

void QNetworkSessionPrivateImpl::migrate()
{
}

void QNetworkSessionPrivateImpl::accept()
{
}

void QNetworkSessionPrivateImpl::ignore()
{
}

void QNetworkSessionPrivateImpl::reject()
{
}

#ifndef QT_NO_NETWORKINTERFACE
QNetworkInterface QNetworkSessionPrivateImpl::currentInterface() const
{
    if (!publicConfig.isValid() || !engine || state != QNetworkSession::Connected)
        return QNetworkInterface();

    QString interface = engine->getInterfaceFromId(activeConfig.identifier());

    if (interface.isEmpty())
        return QNetworkInterface();
    return QNetworkInterface::interfaceFromName(interface);
}
#endif

QVariant QNetworkSessionPrivateImpl::sessionProperty(const QString &key) const
{
    if (key == QLatin1String("AutoCloseSessionTimeout")) {
        if (engine && engine->requiresPolling() &&
            !(engine->capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces)) {
            if (sessionTimeout >= 0)
                return sessionTimeout * 10000;
            else
                return -1;
        }
    }

    return QVariant();
}

void QNetworkSessionPrivateImpl::setSessionProperty(const QString &key, const QVariant &value)
{
    if (key == QLatin1String("AutoCloseSessionTimeout")) {
        if (engine && engine->requiresPolling() &&
            !(engine->capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces)) {
            int timeout = value.toInt();
            if (timeout >= 0) {
                connect(engine, SIGNAL(updateCompleted()),
                        this, SLOT(decrementTimeout()), Qt::UniqueConnection);
                sessionTimeout = timeout / 10000;   // convert to poll intervals
            } else {
                disconnect(engine, SIGNAL(updateCompleted()), this, SLOT(decrementTimeout()));
                sessionTimeout = -1;
            }
        }
    }
}

QString QNetworkSessionPrivateImpl::errorString() const
{
    switch (lastError) {
    case QNetworkSession::UnknownSessionError:
        return tr("Unknown session error.");
    case QNetworkSession::SessionAbortedError:
        return tr("The session was aborted by the user or system.");
    case QNetworkSession::OperationNotSupportedError:
        return tr("The requested operation is not supported by the system.");
    case QNetworkSession::InvalidConfigurationError:
        return tr("The specified configuration cannot be used.");
    case QNetworkSession::RoamingError:
        return tr("Roaming was aborted or is not possible.");

    }

    return QString();
}

QNetworkSession::SessionError QNetworkSessionPrivateImpl::error() const
{
    return lastError;
}

quint64 QNetworkSessionPrivateImpl::bytesWritten() const
{
    if (engine && state == QNetworkSession::Connected)
        return engine->bytesWritten(activeConfig.identifier());
    else
        return Q_UINT64_C(0);
}

quint64 QNetworkSessionPrivateImpl::bytesReceived() const
{
    if (engine && state == QNetworkSession::Connected)
        return engine->bytesReceived(activeConfig.identifier());
    else
        return Q_UINT64_C(0);
}

quint64 QNetworkSessionPrivateImpl::activeTime() const
{
    if (state == QNetworkSession::Connected && startTime != Q_UINT64_C(0))
        return QDateTime::currentDateTime().toTime_t() - startTime;
    else
        return Q_UINT64_C(0);
}

void QNetworkSessionPrivateImpl::updateStateFromServiceNetwork()
{
    QNetworkSession::State oldState = state;

    foreach (const QNetworkConfiguration &config, serviceConfig.children()) {
        if ((config.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active)
            continue;

        if (activeConfig != config) {
            if (engine) {
                disconnect(engine,
                           SIGNAL(connectionError(QString,QBearerEngineImpl::ConnectionError)),
                           this,
                           SLOT(connectionError(QString,QBearerEngineImpl::ConnectionError)));
            }

            activeConfig = config;
            engine = getEngineFromId(activeConfig.identifier());
            if (engine) {
                connect(engine,
                        SIGNAL(connectionError(QString,QBearerEngineImpl::ConnectionError)),
                        this, SLOT(connectionError(QString,QBearerEngineImpl::ConnectionError)),
                        Qt::QueuedConnection);
            }
            emit newConfigurationActivated();
        }

        state = QNetworkSession::Connected;
        if (state != oldState)
            emit stateChanged(state);

        return;
    }

    if (serviceConfig.children().isEmpty())
        state = QNetworkSession::NotAvailable;
    else
        state = QNetworkSession::Disconnected;

    if (state != oldState)
        emit stateChanged(state);
}

void QNetworkSessionPrivateImpl::updateStateFromActiveConfig()
{
    if (!engine)
        return;

    QNetworkSession::State oldState = state;

    state = engine->sessionStateForId(activeConfig.identifier());

    bool oldActive = isOpen;
    isOpen = (state == QNetworkSession::Connected) ? opened : false;

    if (!oldActive && isOpen)
        emit quitPendingWaitsForOpened();
    if (oldActive && !isOpen)
        emit closed();

    if (oldState != state)
        emit stateChanged(state);
}

void QNetworkSessionPrivateImpl::networkConfigurationsChanged()
{
    if (serviceConfig.isValid())
        updateStateFromServiceNetwork();
    else
        updateStateFromActiveConfig();

    startTime = engine->startTime(activeConfig.identifier());
}

void QNetworkSessionPrivateImpl::configurationChanged(QNetworkConfigurationPrivatePointer config)
{
    if (serviceConfig.isValid() &&
        (config->id == serviceConfig.identifier() || config->id == activeConfig.identifier())) {
        updateStateFromServiceNetwork();
    } else if (config->id == activeConfig.identifier()) {
        updateStateFromActiveConfig();
    }
}

void QNetworkSessionPrivateImpl::forcedSessionClose(const QNetworkConfiguration &config)
{
    if (activeConfig == config) {
        opened = false;
        isOpen = false;

        emit closed();

        lastError = QNetworkSession::SessionAbortedError;
        emit QNetworkSessionPrivate::error(lastError);
    }
}

void QNetworkSessionPrivateImpl::connectionError(const QString &id,
                                                 QBearerEngineImpl::ConnectionError error)
{
    if (activeConfig.identifier() == id) {
        networkConfigurationsChanged();
        switch (error) {
        case QBearerEngineImpl::OperationNotSupported:
            lastError = QNetworkSession::OperationNotSupportedError;
            opened = false;
            break;
        case QBearerEngineImpl::InterfaceLookupError:
        case QBearerEngineImpl::ConnectError:
        case QBearerEngineImpl::DisconnectionError:
        default:
            lastError = QNetworkSession::UnknownSessionError;
        }

        emit QNetworkSessionPrivate::error(lastError);
    }
}

void QNetworkSessionPrivateImpl::decrementTimeout()
{
    if (--sessionTimeout <= 0) {
        disconnect(engine, SIGNAL(updateCompleted()), this, SLOT(decrementTimeout()));
        sessionTimeout = -1;
        close();
    }
}

QT_END_NAMESPACE

#endif // QT_NO_BEARERMANAGEMENT
