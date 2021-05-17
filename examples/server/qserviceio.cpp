#include "qserviceio.h"


QObjectLinkConnection::QObjectLinkConnection(SourceRegistry* registry, QWebSocket *socket)
    : QObject(socket)
    , m_socket(socket)
    , m_service(registry)
{
    m_service.onLog(m_log.logFunc());
    registry->onLog(m_log.logFunc());
    connect(m_socket, &QWebSocket::textMessageReceived, this, &QObjectLinkConnection::handleMessage);
    WriteMessageFunc writeFunc = [this](std::string msg) {
        writeMessage(msg);
    };
    m_service.onWrite(writeFunc);
}

void QObjectLinkConnection::writeMessage(const std::string msg)
{
    qDebug() << Q_FUNC_INFO << QString::fromStdString(msg);
    if(m_socket) {
        m_socket->sendTextMessage(QString::fromStdString(msg));
    }
}

void QObjectLinkConnection::handleMessage(const QString &msg)
{
    qDebug() << Q_FUNC_INFO << msg;

    m_service.handleMessage(msg.toStdString());
}
