// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 ApiGear UG (haftungsbeschränkt)
// Copyright (c) 2022-2026 Epic Games, Inc.

#pragma once

#include <QtCore>
#include <QtWebSockets>
#include "olink/remotenode.h"
#include "olink/consolelogger.h"

using namespace ApiGear::ObjectLink;

class OLinkHost :public QObject
{
    Q_OBJECT
public:
    explicit OLinkHost(ApiGear::ObjectLink::RemoteRegistry& registry, QObject *parent);
    virtual ~OLinkHost() override;
    void listen(const QString& host, int port);
    void onNewConnection();
    void onClosed();
    const QString &name() const;

    RemoteRegistry &registry();

private:
    QWebSocketServer* m_wss;
    ApiGear::ObjectLink::RemoteRegistry* m_registry;
};
