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
    nlohmann::json olinkInvoke(std::string name, nlohmann::json args) override;
    void olinkSetProperty(std::string name, nlohmann::json value) override;
    void olinkLinked(std::string name, IRemoteNode *node) override;
    void olinkUnlinked(std::string name) override;
    nlohmann::json olinkCollectProperties() override;
private:
    IRemoteNode* m_node;
    int m_total;
    RemoteRegistry* m_registry;
};

