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
#pragma once

#include <QtCore>
#include <QtWebSockets>
#include "olink/core/types.h"
#include "olink/sinktypes.h"
#include "olink/client.h"
#include "olink/core/consolelogger.h"

using namespace ApiGear::ObjectLink;

class QObjectLinkClient
        : public QObject
{
    Q_OBJECT
public:
    explicit QObjectLinkClient(const QString &name, QWebSocket *socket=nullptr, QObject *parent=nullptr);
    virtual ~QObjectLinkClient() override;
    void connectToHost(QUrl url);
    SinkRegistry& registry();
    ClientIO &client();
    void link(const QString &name);
public:

    void onConnected();
    void onDisconnected();
    void handleTextMessage(const QString& message);
    void processMessages();
    const QString &name() const;

private:
    QWebSocket *m_socket;
    ClientIO m_client;
    QQueue<std::string> m_protocol;
    QString m_name;
    ConsoleLogger m_logger;
};
