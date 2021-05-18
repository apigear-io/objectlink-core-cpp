#include "qserviceio.h"


SourceConnection::SourceConnection(const QString &name, QWebSocket *socket)
    : QObject(socket)
    , m_socket(socket)
    , m_adapter(name.toStdString())
{
    m_adapter.onLog(m_log.logFunc());
    connect(m_socket, &QWebSocket::textMessageReceived, this, &SourceConnection::handleMessage);
    WriteMessageFunc writeFunc = [this](std::string msg) {
        writeMessage(msg);
    };
    m_adapter.onWrite(writeFunc);
}

void SourceConnection::writeMessage(const std::string msg)
{
    qDebug() << Q_FUNC_INFO << QString::fromStdString(msg);
    if(m_socket) {
        m_socket->sendTextMessage(QString::fromStdString(msg));
    }
}

void SourceConnection::handleMessage(const QString &msg)
{
    qDebug() << Q_FUNC_INFO << msg;

    m_adapter.handleMessage(msg.toStdString());
}
