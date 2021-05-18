/*
* MIT License
*
* Copyright (c) 2021 ApiGear
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "qserver.h"

#include "../app/qclientio.h"
#include "qserviceio.h"

using namespace ApiGear::ObjectLink;



QObjectLinkHost::QObjectLinkHost(const QString& name, QObject *parent)
    : QObject(parent)
    , m_nodeName(name)
    , m_wss(new QWebSocketServer("olink", QWebSocketServer::NonSecureMode, this))
    , m_registry(name.toStdString())
{
    m_registry.onLog(m_log.logFunc());
}

QObjectLinkHost::~QObjectLinkHost()
{
}

void QObjectLinkHost::listen(const QString& host, int port)
{
    qDebug() << "wss.listen()";
    m_wss->listen(QHostAddress(host), quint16(port));
    qDebug() << m_wss->serverAddress() << m_wss->serverPort();
    connect(m_wss, &QWebSocketServer::newConnection, this, &QObjectLinkHost::onNewConnection);
    connect(m_wss, &QWebSocketServer::closed, this, &QObjectLinkHost::onClosed);
}

void QObjectLinkHost::onNewConnection()
{
    qDebug() << "wss.newConnection()";
    QWebSocket *ws = m_wss->nextPendingConnection();
    new QObjectLinkConnection(m_nodeName, ws);
}

void QObjectLinkHost::onClosed()
{
    qDebug() << "wss.closed()";
}

SourceNode &QObjectLinkHost::registry()
{
    return m_registry;
}

const QString &QObjectLinkHost::nodeName() const
{
    return m_nodeName;
}

