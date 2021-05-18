#pragma once

#include <QtCore>
#include <QtWebSockets>

#include "olink/core/consolelogger.h"
#include "olink/sourcetypes.h"
#include "olink/sourcelink.h"

using namespace ApiGear::ObjectLink;

class QObjectLinkConnection : public QObject
{
    Q_OBJECT
public:
    explicit QObjectLinkConnection(const QString& name, QWebSocket* socket);
    void writeMessage(const std::string msg);
    void handleMessage(const QString& msg);
private:
    QWebSocket* m_socket;
    SourceLink m_link;
    ConsoleLogger m_log;

};
