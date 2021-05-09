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
#include "olink/types.h"

using namespace ApiGear::ObjectLink;

namespace ApiGear { namespace ObjectLink {
class Session;
} }

class QObjectLink :public QObject, public IMessageWriter, public ISessionListener
{
    Q_OBJECT
public:
    explicit QObjectLink(QObject *parent=nullptr);
    virtual ~QObjectLink() override;
    Q_INVOKABLE void open(QUrl url);
    Q_INVOKABLE void doCall();
    Q_INVOKABLE void doRegister();
    Q_INVOKABLE void doUnregister();

    Q_INVOKABLE void doPublish();
    Q_INVOKABLE void doSubscribe();
    Q_INVOKABLE void doUnSubscribe();

public:
    void onConnected();
    void onDisconnected();
    void handleTextMessage(const QString& message);
    void onError(std::string error) override;
    void onEvent(std::string topic, Arguments args, ArgumentsKw kwargs) override;
    void onJoin() override;
    void writeMessage(std::string message) override;
private:
    QWebSocket *m_socket;
    Session *m_session;
};
