#pragma once

#include "olink/clientnode.h"
#include "olink/iobjectsink.h"
#include "olink/clientregistry.h"
#include <iostream>

using namespace ApiGear::ObjectLink;

class MockSink: public IObjectSink {
public:
    MockSink(ClientRegistry& registry)
        : m_client(nullptr)
        , m_ready(false)
        , m_registry(registry)
    {
        m_registry.addObject(*this);
    }
    virtual ~MockSink() override {
        m_registry.removeObject(olinkObjectName());
    }
    IClientNode *client() const {
        assert(m_client);
        return m_client;
    }
    void invoke(std::string name, nlohmann::json args) {
        m_client->invokeRemote(name, args, [this](InvokeReplyArg arg) {
            m_events.push_back({{ "type", "invokeReply"}, { "name", arg.methodId}, { "value", arg.value }});
        });
    }
public:
    std::string olinkObjectName() override {
        return "demo.Mock";
    }
    void olinkOnSignal(const std::string& name, const nlohmann::json& args) override {
        std::cout << "MockSink.olinkOnSignal" << name  << args.dump() << std::endl;
        m_events.push_back({ {"type", "signal"}, { "name", "name"}, { "args", args } });

    }
    void olinkOnPropertyChanged(const std::string& name, const nlohmann::json& value) override {
        std::cout << "MockSink.olinkOnPropertyChanged" << name << value.dump() << std::endl;
        std::string path = Name::getMemberName(name);
        m_properties[path] = value;
        m_events.push_back({ {"type", "propertyChange"}, { "name", "name"}, { "value", value }});
    }
    void olinkOnInit(const std::string& name, const nlohmann::json& props, IClientNode *client) override {
        std::cout << "MockSink.olinkOnInit: " << name << props.dump() << std::endl;
        m_client = client;
        m_ready = true;
        m_events.push_back({{ "type", "init",}, { "name", name}});
        m_properties = props;
    }
    void olinkOnRelease() override {
        m_ready = false;
        m_client = nullptr;
    }
public:
    std::list<nlohmann::json> m_events;
    nlohmann::json m_properties;
    IClientNode *m_client;
    bool m_ready;
    ClientRegistry& m_registry;

};
