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
#include "qolink.h"
#include "wamp/session.h"
#include "wamp/stdoutlogger.h"

using namespace ApiGear::ObjectLink;



QObjectLink::QObjectLink(QObject *parent)
    : QObject(parent)
    , m_socket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , m_session(new Session(this, this, new StdoutLogger(), MessageFormat::JSON))
{
    connect(m_socket, &QWebSocket::connected, this, &QObjectLink::onConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &QObjectLink::onDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &QObjectLink::handleTextMessage);
}

QObjectLink::~QObjectLink()
{
    delete m_session;
}

void QObjectLink::open(QUrl url)
{
    m_socket->open(QUrl(url));
}

void QObjectLink::onConnected()
{
    qDebug() << "socket connected";
    m_session->init("realm1");
}

void QObjectLink::onDisconnected()
{
    qDebug() << "socket disconnected";
}

void QObjectLink::handleTextMessage(const QString &message)
{
    m_session->handleMessage(message.toStdString());
}

void QObjectLink::doCall()
{
    ResponseFunc func = [](ResponseArg response) { qDebug() << "response.args"; };
    m_session->doCall("increment", {}, {{}}, func);
}

void QObjectLink::doRegister()
{
    ProcedureFunc func = [](ProcedureArg arg) { qDebug() << "response.args"; };
    m_session->doRegister("increment", func);
}

void QObjectLink::doUnregister()
{
    m_session->doUnregister("increment");
}

void QObjectLink::doPublish()
{
    m_session->doPublish("shutdown", {5}, {{}});
}

void QObjectLink::doSubscribe()
{
    EventFunc func = [](EventArg arg) { qDebug() << "event.args"; };
    m_session->doSubscribe("shutdown", func);
}

void QObjectLink::doUnSubscribe()
{
    m_session->doUnsubscribe("shutdown");
}


void QObjectLink::writeMessage(std::string message)
{
    qDebug() << "write message to socket" << QString::fromStdString(message);
    // if we are using JSON we need to use txt message
    // otherwise binary messages
    //    m_socket->sendBinaryMessage(QByteArray::fromStdString(message));
    m_socket->sendTextMessage(QString::fromStdString(message));
}


void QObjectLink::onError(std::string error)
{
    qDebug() << "onError"; //  << error;
}

void QObjectLink::onEvent(std::string topic, Arguments args, ArgumentsKw kwargs)
{
    qDebug() << "onEvent"; // << topic; // json(args).dump() << json(kwargs).dump();
}


void QObjectLink::onJoin()
{
    qDebug() << "on join";
}
