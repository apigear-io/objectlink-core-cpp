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
#include "qclientio.h"
#include "olink/client.h"

using namespace ApiGear::ObjectLink;

QObjectLinkClient::QObjectLinkClient(const QString& name, QWebSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket ? socket : new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , m_client(name.toStdString())
{
    m_client.onLog(m_logger.logFunc());
    connect(m_socket, &QWebSocket::connected, this, &QObjectLinkClient::onConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &QObjectLinkClient::onDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &QObjectLinkClient::handleTextMessage);
    WriteMessageFunc func = [this](std::string msg) {
        m_protocol << msg;
        processMessages();
    };
    m_client.onWrite(func);
}

QObjectLinkClient::~QObjectLinkClient()
{
}


void QObjectLinkClient::connectToHost(QUrl url)
{
    m_socket->open(QUrl(url));
}

SinkRegistry &QObjectLinkClient::registry()
{
    return m_client.registry();
}

ClientIO &QObjectLinkClient::client()
{
    return m_client;
}

void QObjectLinkClient::link(const QString &name)
{
    m_client.link(name.toStdString());
}


void QObjectLinkClient::onConnected()
{
    qDebug() << "socket connected";
    processMessages();
}

void QObjectLinkClient::onDisconnected()
{
    qDebug() << "socket disconnected";
}

void QObjectLinkClient::handleTextMessage(const QString &message)
{
    m_client.handleMessage(message.toStdString());
}


void QObjectLinkClient::processMessages()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        while(!m_protocol.isEmpty()) {
            // if we are using JSON we need to use txt message
            // otherwise binary messages
            //    m_socket->sendBinaryMessage(QByteArray::fromStdString(message));
            const QString& msg = QString::fromStdString(m_protocol.dequeue());
            qDebug() << "write message to socket" << msg;
            m_socket->sendTextMessage(msg);
        }
    }

}


const QString &QObjectLinkClient::name() const
{
    return m_name;
}
