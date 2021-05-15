#include "qobjectlinkconnection.h"


QObjectLinkConnection::QObjectLinkConnection(SourceRegistry* registry, QWebSocket *socket)
    : QObject(socket)
    , m_socket(socket)
    , m_service(registry)
{
    connect(m_socket, &QWebSocket::textMessageReceived, this, &QObjectLinkConnection::handleMessage);
    WriteMessageFunc writeFunc = [this](std::string msg) {
        writeMessage(msg);
    };
    m_service.onWrite(writeFunc);
}

void QObjectLinkConnection::writeMessage(const std::string msg)
{
    if(m_socket) {
        m_socket->sendTextMessage(QString::fromStdString(msg));
    }
}

void QObjectLinkConnection::handleMessage(const QString &msg)
{
    m_service.handleMessage(msg.toStdString());
}
