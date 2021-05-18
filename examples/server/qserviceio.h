#pragma once

#include <QtCore>
#include <QtWebSockets>

#include "olink/core/consolelogger.h"
#include "olink/source/sourcetypes.h"
#include "olink/source/sourcenode.h"

using namespace ApiGear::ObjectLink;

class SourceConnection : public QObject
{
    Q_OBJECT
public:
    explicit SourceConnection(const QString& name, QWebSocket* socket);
    void writeMessage(const std::string msg);
    void handleMessage(const QString& msg);
private:
    QWebSocket* m_socket;
    ObjectSourceNode m_adapter;
    ConsoleLogger m_log;

};
