#pragma once
#include "olink/remotenode.h"

using namespace ApiGear::ObjectLink;


class CalcSource: public IObjectSource {
public:
    CalcSource(RemoteRegistry& registry);
    virtual ~CalcSource() override;

    IRemoteNode *node() const;

    int add(int value);

    int sub(int value);
    void clear();
    void notifyShutdown(int timeout);
    // IObjectSource interface
public:
    std::string olinkObjectName() override;
    nlohmann::json olinkInvoke(const std::string& name, const nlohmann::json& args) override;
    void olinkSetProperty(const std::string& name, const nlohmann::json& value) override;
    void olinkLinked(const std::string& name, IRemoteNode *node) override;
    void olinkUnlinked(const std::string& name) override;
    nlohmann::json olinkCollectProperties() override;
private:
    IRemoteNode* m_node;
    int m_total;
    RemoteRegistry* m_registry;
};

