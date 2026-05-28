// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 ApiGear UG (haftungsbeschränkt)
// Copyright (c) 2022-2026 Epic Games, Inc.

#include "olinkhost.h"

#include "olinkremote.h"

using namespace ApiGear::ObjectLink;



OLinkHost::OLinkHost(ApiGear::ObjectLink::RemoteRegistry& registry, QObject *parent)
    : QObject(parent)
    , m_wss(new QWebSocketServer("olink", QWebSocketServer::NonSecureMode, this))
    , m_registry(&registry)
{
}

OLinkHost::~OLinkHost()
{
}

void OLinkHost::listen(const QString& host, int port)
{
    qDebug() << "wss.listen()";
    m_wss->listen(QHostAddress(host), quint16(port));
    qDebug() << m_wss->serverAddress() << m_wss->serverPort();
    connect(m_wss, &QWebSocketServer::newConnection, this, &OLinkHost::onNewConnection);
    connect(m_wss, &QWebSocketServer::closed, this, &OLinkHost::onClosed);
}

void OLinkHost::onNewConnection()
{
    qDebug() << "wss.newConnection()";
    QWebSocket *ws = m_wss->nextPendingConnection();
    new OLinkRemote(ws, *m_registry);
}

void OLinkHost::onClosed()
{
    qDebug() << "wss.closed()";
}


RemoteRegistry &OLinkHost::registry()
{
    return *m_registry;
}

