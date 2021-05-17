#pragma once

#include <QtCore>
#include <QtWebSockets>

#include "olink/core/consolelogger.h"
#include "olink/sourcetypes.h"
#include "olink/service.h"

using namespace ApiGear::ObjectLink;

class QObjectLinkConnection : public QObject
{
    Q_OBJECT
public:
    explicit QObjectLinkConnection(SourceRegistry* registry, QWebSocket* socket);
    void writeMessage(const std::string msg);
    void handleMessage(const QString& msg);
private:
    QWebSocket* m_socket;
    ServiceIO m_service;
    ConsoleLogger m_log;

};
