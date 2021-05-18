#pragma once

#include "sinktypes.h"
#include "olink/shared/basenode.h"

namespace ApiGear { namespace ObjectLink {

class ObjectSinkRegistry;

class ObjectSinkNode : public BaseNode, public IObjectSinkNode
{
public:
    ObjectSinkNode(std::string name);
    ObjectSinkRegistry* registry();
public: // IObjectSinkNode
    void link(std::string name) override;
    void unlink(std::string name) override;
    void invoke(std::string name, json args=json{}, InvokeReplyFunc func=nullptr) override;
    void setProperty(std::string name, json value) override;
protected: // IMessageListener
    void handleInit(std::string name, json props) override;
    void handlePropertyChange(std::string name, json value) override;
    void handleInvokeReply(int requestId, std::string name, json value) override;
    void handleSignal(std::string name, json args) override;
    void handleError(int msgType, int requestId, std::string error) override;
protected:
    int nextId();
private:
    int m_nextId;
    std::map<int,InvokeReplyFunc> m_invokesPending;
};

} } // ApiGear::ObjectLink
