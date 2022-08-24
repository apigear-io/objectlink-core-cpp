#pragma once

#include "olink/clientnode.h"
#include "olink/iobjectsink.h"
#include "olink/clientregistry.h"
#include <iostream>

using namespace ApiGear::ObjectLink;

class CalcSink: public IObjectSink {
public:
    CalcSink(ClientRegistry& registry)
        : m_client(nullptr)
        , m_registry(registry)
        , m_total(0)
        , m_ready(false)
    {
        m_registry.addObject(*this);
    }
    virtual ~CalcSink() override {
        m_registry.removeObject(olinkObjectName());
    }
    int total() const {
        return m_total;
    }
    void setTotal(int value) {
        client()->setRemoteProperty("demo.Calc/total", value);
    }
    int add(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            std::cout << "invoke reply" << arg.name << arg.value.dump();
        };
        client()->invokeRemote("demo.Calc/add", { a }, func);

        return -1;
    }
    int sub(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            std::cout << "invoke reply " << arg.name << arg.value.dump();
        };
        client()->invokeRemote("demo.Calc/sub", { a }, func);
        return -1;
    }
    IClientNode *client() const {
        assert(m_client);
        return m_client;
    }
    bool isReady() const {
        return m_ready;
    }
    // IClientObjectHandler interface
public:
    std::string olinkObjectName() override {
        return "demo.Calc";
    }
    void olinkOnSignal(const std::string& name, const nlohmann::json& args) override {
        std::cout << "onSignal" << name  << args.dump() << std::endl;
        events.push_back({name, args});

    }
    void olinkOnPropertyChanged(const std::string& name, const nlohmann::json& value) override {
        std::cout << "onPropertyChanged" << name << value.dump() << std::endl;
        std::string path = Name::getMemberName(name);
        if(path == "total") {
            int total = value.get<int>();
            if(m_total != total) {
                m_total = total;
            }
        }

    }
    void olinkOnInit(const std::string& name, const nlohmann::json& props, IClientNode* client) override {
        std::cout << "CalcSink.olinkOnInit: " << name << props.dump() << std::endl;
        m_client = client;
        m_ready = true;
        if(props.contains("total")) {
            int total = props["total"].get<int>();
            if(m_total != total) {
                m_total = total;
            }
        }
    }
    void olinkOnRelease() override {
        m_ready = false;
        m_client = nullptr;
    }
public:
    std::list<nlohmann::json> events;
private:
    IClientNode *m_client;
    ClientRegistry& m_registry;
    int m_total;
    bool m_ready;

};
