// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 ApiGear UG (haftungsbeschränkt)
// Copyright (c) 2022-2026 Epic Games, Inc.

#include "olinkclient.h"
#include "olink/clientnode.h"
#include "olink/clientregistry.h"

using namespace ApiGear::ObjectLink;

OLinkClient::OLinkClient(QWebSocket *socket, ApiGear::ObjectLink::ClientRegistry& registry, QObject *parent)
    : QObject(parent)
    , m_socket(socket ? socket : new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , m_registry(&registry)
    , m_node(ClientNode::create(registry))
{
    m_node->onLog(ConsoleLogger::logFunc());
    connect(m_socket, &QWebSocket::connected, this, &OLinkClient::onConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &OLinkClient::onDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &OLinkClient::handleTextMessage);
    WriteMessageFunc func = [this](std::string msg) {
        m_queue << msg;
        processMessages();
    };
    m_node->onWrite(func);
}

OLinkClient::~OLinkClient()
{
}


void OLinkClient::connectToHost(QUrl url)
{
    qDebug() << Q_FUNC_INFO;
    m_socket->open(QUrl(url));
}

ClientNode &OLinkClient::node()
{
    return *(m_node);
}

void OLinkClient::linkObjectSource(std::string name)
{
    qDebug() << Q_FUNC_INFO << QString::fromStdString(name);
    m_node->registry().setNode(m_node->getNodeId(), name);
    m_node->linkRemote(name);
}


void OLinkClient::onConnected()
{
    qDebug() << Q_FUNC_INFO;
    processMessages();
}

void OLinkClient::onDisconnected()
{
    qDebug() << Q_FUNC_INFO;
}

void OLinkClient::handleTextMessage(const QString &message)
{
    m_node->handleMessage(message.toStdString());
}


void OLinkClient::processMessages()
{
    qDebug() << Q_FUNC_INFO;
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        while(!m_queue.isEmpty()) {
            // if we are using JSON we need to use txt message
            // otherwise binary messages
            //    m_socket->sendBinaryMessage(QByteArray::fromStdString(message));
            const QString& msg = QString::fromStdString(m_queue.dequeue());
            qDebug() << "write message to socket: " << msg;
            m_socket->sendTextMessage(msg);
        }
    }

}
