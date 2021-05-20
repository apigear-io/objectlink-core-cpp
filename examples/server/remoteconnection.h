#pragma once

#include <QtCore>
#include <QtWebSockets>

#include "olink/consolelogger.h"
#include "olink/remotenode.h"

using namespace ApiGear::ObjectLink;

class RemoteConnection : public QObject
{
    Q_OBJECT
public:
    explicit RemoteConnection(QWebSocket* socket);
    void writeMessage(const std::string msg);
    void handleMessage(const QString& msg);
private:
    QWebSocket* m_socket;
    RemoteNode m_node;
    ConsoleLogger m_log;

};
