#include "basenode.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {


// ********************************************************************
// BaseNode
// ********************************************************************


BaseNode::BaseNode()
    : Base()
    , m_writeFunc(nullptr)
    , m_converter(MessageFormat::JSON)
    , m_protocol(this)
{
}

void BaseNode::onWrite(WriteMessageFunc func)
{
    m_writeFunc = func;
}

void BaseNode::emitWrite(nlohmann::json msg)
{
    const std::string& data = m_converter.toString(msg);
    emitLog(LogLevel::Debug, "writeMessage " + msg.dump());
    if(m_writeFunc) {
        m_writeFunc(data);
    } else {
        emitLog(LogLevel::Warning, "no writer set, can not write");
    }
}


void BaseNode::handleMessage(const std::string& data)
{
    const nlohmann::json& j = m_converter.fromString(data);
    m_protocol.handleMessage(j);
}

void BaseNode::handleLink(const std::string& interfaceId)
{
    std::cout << "not implemented " << __func__ << interfaceId << std::endl;
}

void BaseNode::handleUnlink(const std::string& interfaceId)
{
    std::cout << "not implemented " << __func__ << interfaceId << std::endl;
}

void BaseNode::handleInvoke(int requestId, const std::string& methodId, const nlohmann::json& args)
{
    std::cout << "not implemented " << __func__ << requestId << methodId << args.dump() << std::endl;
}

void BaseNode::handleSetProperty(const std::string& propertyId, const nlohmann::json& value)
{
    std::cout << "not implemented " << __func__ << propertyId << value.dump() << std::endl;
}


void BaseNode::handleInit(const std::string& interfaceId, const nlohmann::json& props)
{
    std::cout << "not implemented " << __func__ << interfaceId << props.dump() << std::endl;
}

void BaseNode::handleInvokeReply(int requestId, const std::string& methodId, const nlohmann::json& value)
{
    std::cout << "not implemented " << __func__ << requestId << methodId << value.dump() << std::endl;
}

void BaseNode::handleSignal(const std::string& signalId, const nlohmann::json& args)
{
    std::cout << "not implemented " << __func__ << signalId << args.dump()  << std::endl;
}

void BaseNode::handlePropertyChange(const std::string& propertyId, const nlohmann::json& value)
{
    std::cout << "not implemented " << __func__ << propertyId << value.dump() << std::endl;
}

void BaseNode::handleError(int msgType, int requestId, const std::string& error)
{
    std::cout << "not implemented " << __func__ << msgType << requestId << error << std::endl;
}





} } // ApiGear::ObjectLink
