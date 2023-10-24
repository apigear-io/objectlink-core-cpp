#pragma once

#include "olink/clientnode.h"
#include "olink/iobjectsink.h"
#include "olink/clientregistry.h"
#include <iostream>
#include <list>

using namespace ApiGear::ObjectLink;

class CalcSink: public IObjectSink {
public:
    CalcSink(ClientRegistry& registry)
        : m_client(nullptr)
        , m_registry(registry)
        , m_total(0)
        , m_ready(false)
    {}
    virtual ~CalcSink() override {
        m_registry.removeSink(olinkObjectName());
    }
    int total() const {
        return m_total;
    }
    void setTotal(int value) {
        client()->setRemoteProperty("demo.Calc/total", propertyToContent(value));
    }
    int add(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            std::cout << "invoke reply" << arg.methodId << toString(arg.value);
        };
        client()->invokeRemote("demo.Calc/add", argumentsToContent(a), func);

        return -1;
    }
    int sub(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            std::cout << "invoke reply " << arg.methodId << toString(arg.value);
        };
        client()->invokeRemote("demo.Calc/sub", argumentsToContent(a), func);
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
    void olinkOnSignal(const std::string& name, const OLinkContent& args) override {
        std::cout << "onSignal" << name  << toString(args) << std::endl;
        OLinContentStreamReader reader(args);
        int signalArgValue;
        reader.read(signalArgValue);
        events.push_back(std::make_pair(name, signalArgValue));

    }
    void olinkOnPropertyChanged(const std::string& name, const OLinkContent& value) override {
        std::cout << "onPropertyChanged" << name << toString(value) << std::endl;
        std::string path = Name::getMemberName(name);
        if(path == "total") {
            int total = 0;
            readValue(value, total);
            if(m_total != total) {
                m_total = total;
            }
        }

    }
    void olinkOnInit(const std::string& name, const OLinkContent& props, IClientNode* client) override {
        std::cout << "CalcSink.olinkOnInit: " << name << toString(props) << std::endl;
        m_client = client;
        m_ready = true;
        InitialProperty expectTotalProperty;
        OLinContentStreamReader reader(props);
        reader.read(expectTotalProperty);
        if(expectTotalProperty.propertyName == "total") {
            int total = 0;
            readValue(expectTotalProperty, total);
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
    std::list<std::pair<std::string, int>> events;
private:
    IClientNode *m_client;
    ClientRegistry& m_registry;
    int m_total;
    bool m_ready;

};
