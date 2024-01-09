#include "basenode.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {

namespace
{
static const std::string notImplementedLog = "not implemented ";
}

void BaseNode::onWrite(WriteMessageFunc func, std::shared_ptr<IMessageSerializer> serializer)
{
    m_writeFunc = func;
    m_serializer = serializer;
}

void BaseNode::emitWrite(const OLinkMessage& msg)
{
    static const std::string writeMessageLog = "writeMessage: ";
    emitLogWithPayload(LogLevel::Debug, msg, writeMessageLog);
    if(m_writeFunc && m_serializer) {
        m_writeFunc(m_serializer->toNetworkFormat(msg));
    } else {
        static const std::string noWriterSetLog = "Messages are not sent if the write function is not set";
        emitLog(LogLevel::Warning, noWriterSetLog);
    }
}

void BaseNode::handleMessage(const std::string& data)
{
    auto msg = m_serializer->fromNetworkFormat(data);
    auto deserializer = m_serializer->createReader(msg);
    m_protocol.handleMessage(*(deserializer.get()), *this);
}

void BaseNode::handleLink(const std::string& objectId)
{
    emitLog(LogLevel::Warning, notImplementedLog, std::string(__func__), objectId);
}

void BaseNode::handleUnlink(const std::string& objectId)
{
    emitLog(LogLevel::Warning, notImplementedLog, std::string(__func__), objectId);
}

void BaseNode::handleInvoke(int, const std::string& methodId, const OLinkContent& args)
{
    emitLogWithPayload(LogLevel::Warning, args, notImplementedLog, std::string(__func__), methodId, " args ");
}

void BaseNode::handleSetProperty(const std::string& propertyId, const OLinkContent& value)
{
    emitLogWithPayload(LogLevel::Warning, value, notImplementedLog, std::string(__func__), propertyId, " value ");
}

void BaseNode::handleInit(const std::string& objectId, const OLinkContent& props)
{
    emitLogWithPayload(LogLevel::Warning, props, notImplementedLog, std::string(__func__), objectId, " props ");
}

void BaseNode::handleInvokeReply(int requestId, const std::string& methodId, const OLinkContent& value)
{
    emitLogWithPayload(LogLevel::Warning, value, notImplementedLog, std::string(__func__), methodId, " requestId ", std::to_string(requestId), " value ");
}

void BaseNode::handleSignal(const std::string& signalId, const OLinkContent& args)
{
    emitLogWithPayload(LogLevel::Warning, args, notImplementedLog, std::string(__func__), signalId, " args ");
}

void BaseNode::handlePropertyChange(const std::string& propertyId, const OLinkContent& value)
{
    emitLogWithPayload(LogLevel::Warning, value, notImplementedLog, std::string(__func__), propertyId, " value ");
}

void BaseNode::handleError(MsgType, int requestId, const std::string& error)
{
    emitLog(LogLevel::Warning, notImplementedLog, std::string(__func__), " requestId ", std::to_string(requestId), " error ", error);
}

std::shared_ptr<IMessageSerializer> BaseNode::getSerializer()
{
    return m_serializer;
}

} } // ApiGear::ObjectLink
