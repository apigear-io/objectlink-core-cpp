#pragma once

#include "protocol.h"
#include "types.h"
#include "olink_common.h"
#include "nlohmann/json.hpp"
#include <cstring>

namespace ApiGear { namespace ObjectLink {

/**
 * @brief A base class for client and remote nodes
 * Adds default handling of listener calls and a emits write message when required
 */
class OLINK_EXPORT BaseNode: public Base, public IProtocolListener, public IMessageHandler {
public:
    BaseNode();
    void onWrite(WriteMessageFunc func);
    virtual void emitWrite(nlohmann::json j);
public: // IMessageHandler
    void handleMessage(const std::string& data) override;
public: // IProtocolListener
    void handleLink(const std::string& interfaceId) override;
    void handleUnlink(const std::string& interfaceId) override;
    void handleInvoke(int requestId, const std::string& methodId, const nlohmann::json& args) override;
    void handleSetProperty(const std::string& propertyId, const nlohmann::json& value) override;
    void handleInit(const std::string& interfaceId, const nlohmann::json& props) override;
    void handleInvokeReply(int requestId, const std::string& methodId, const nlohmann::json& value) override;
    void handleSignal(const std::string& signalId, const nlohmann::json& args) override;
    void handlePropertyChange(const std::string& propertyId, const nlohmann::json& value) override;
    void handleError(int msgType, int requestId, const std::string& error) override;
private:
    WriteMessageFunc m_writeFunc;
    MessageConverter m_converter;
    Protocol m_protocol;
};

} } // ApiGear::ObjectLink
