#include "olinkremote.h"


OLinkRemote::OLinkRemote(QWebSocket *socket, ApiGear::ObjectLink::RemoteRegistry& registry)
    : QObject(socket)
    , m_socket(socket)
    , m_registry(&registry)
    , m_node(RemoteNode::createRemoteNode(registry))
{
    m_node->onLog(ConsoleLogger::logFunc());
    connect(m_socket, &QWebSocket::textMessageReceived, this, &OLinkRemote::handleMessage);
    WriteMessageFunc writeFunc = [this](std::string msg) {
        writeMessage(msg);
    };
    m_node->onWrite(writeFunc);
}

void OLinkRemote::writeMessage(const std::string msg)
{
    qDebug() << Q_FUNC_INFO << QString::fromStdString(msg);
    if(m_socket) {
        m_socket->sendTextMessage(QString::fromStdString(msg));
    }
}

void OLinkRemote::handleMessage(const QString &msg)
{
    qDebug() << Q_FUNC_INFO << msg;
    m_node->handleMessage(msg.toStdString());
}
