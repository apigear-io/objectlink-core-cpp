// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 ApiGear UG (haftungsbeschränkt)
// Copyright (c) 2022-2026 Epic Games, Inc.

#pragma once

#include <QtCore>
#include <QtWebSockets>
#include "olink/clientnode.h"
#include "olink/consolelogger.h"

#include <memory>

using namespace ApiGear::ObjectLink;

class OLinkClient
        : public QObject
{
    Q_OBJECT
public:
    explicit OLinkClient(QWebSocket *socket, ApiGear::ObjectLink::ClientRegistry& registry, QObject *parent);
    virtual ~OLinkClient() override;
    void connectToHost(QUrl url);
    ClientNode &node();
    void linkObjectSource(std::string name);
public:

    void onConnected();
    void onDisconnected();
    void handleTextMessage(const QString& message);
    void processMessages();

private:
    QWebSocket *m_socket;
    ApiGear::ObjectLink::ClientRegistry* m_registry;
    std::shared_ptr<ClientNode> m_node;
    QQueue<std::string> m_queue;
};
