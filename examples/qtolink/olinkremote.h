#pragma once

#include <QtCore>
#include <QtWebSockets>

#include "olink/consolelogger.h"
#include "olink/remotenode.h"

using namespace ApiGear::ObjectLink;

class OLinkRemote : public QObject
{
    Q_OBJECT
public:
    explicit OLinkRemote(QWebSocket* socket, ApiGear::ObjectLink::RemoteRegistry& registry);
    void writeMessage(const std::string msg);
    void handleMessage(const QString& msg);
private:
    QWebSocket* m_socket;
    ApiGear::ObjectLink::RemoteRegistry* m_registry;
    RemoteNode m_node;
};
