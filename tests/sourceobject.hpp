#pragma once

#include "olink/remotenode.h"
#include "olink/remoteregistry.h"
#include "olink/iobjectsource.h"
#include <iostream>

using namespace ApiGear::ObjectLink;

class CalcSource: public IObjectSource {
public:
    CalcSource(RemoteRegistry& registry)
        : m_node(nullptr)
        , m_registry(&registry)
        , m_total(1)
    {
    }
    virtual ~CalcSource() override {
        m_registry->removeSource(olinkObjectName());
    }

    IRemoteNode* remoteNode() const {
        assert(m_node);
        return m_node;
    }

    int add(int value) {
        m_total += value;
        remoteNode()->notifyPropertyChange("demo.Calc/total", propertyToContent(m_total));
        m_events.push_back(std::make_pair("demo.Calc/sub", value));
        m_events.push_back(std::make_pair("demo.Calc/total", m_total));
        if(m_total >= 10) {
            remoteNode()->notifySignal("demo.Calc/hitUpper", argumentsToContent( 10 ));
            m_events.push_back({ "demo.Calc/hitUpper", 10 });
        }
        return m_total;
    }

    int sub(int value) {
        m_total -= value;
        remoteNode()->notifyPropertyChange("demo.Calc/total", propertyToContent(m_total));
        m_events.push_back(std::make_pair("demo.Calc/sub", value));
        m_events.push_back(std::make_pair("demo.Calc/total", m_total));
        if(m_total <= 0) {
            remoteNode()->notifySignal("demo.Calc/hitLower", argumentsToContent(0));
            m_events.push_back(std::make_pair("demo.Calc/hitLower", 0));
        }
        return m_total;
    }
    void notifyShutdown(int timeout) {
        remoteNode()->notifySignal("demo.Calc/timeout", argumentsToContent(timeout));
    }
    // IServiceObjectListener interface
public:
    std::string olinkObjectName() override {
        return "demo.Calc";
    }
    OLinkContent olinkInvoke(const std::string& name, const OLinkContent& args) override {
        std::cout << "invoke" << name << toString(args);
        std::string path = Name::getMemberName(name);
        if(path == "add") {
            int a=0;
            OLinContentStreamReader reader(args);
            reader.read(a);
            auto result  = invokeReturnValue(add(a));
            return result;
        }
        return OLinkContent();
    }
    void olinkSetProperty(const std::string& name, const OLinkContent& value) override {
        std::cout << "setProperty" << name << toString(value);
        std::string path = Name::getMemberName(name);
        if(path == "total") {
            int total = 0;
            readValue(value, total);
            if(m_total != total) {
                m_total = total;
                remoteNode()->notifyPropertyChange(name, value);
            }
        }
    }
    void olinkLinked(const std::string& name, IRemoteNode *node) override {
        std::cout << "linked" << name;
        m_node = node;
    }
    void olinkUnlinked(const std::string& name) override
    {
        std::cout << "unlinked" << name;
        m_node = nullptr;
    }
    OLinkContent olinkCollectProperties() override
    {
        return argumentsToContent(toInitialProperty(std::string("total"), m_total));
    }
private:
    IRemoteNode* m_node;
    RemoteRegistry* m_registry;
    int m_total;
    std::list<std::pair<std::string, int>> m_events;
};
