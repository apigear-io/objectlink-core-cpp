#pragma once

#include "olink/clientnode.h"
#include "olink/iobjectsink.h"
#include "olink/clientregistry.h"
#include <iostream>
#include <list>
#include "olink/core/defaultcontentserializer.h"

using namespace ApiGear::ObjectLink;
namespace ContentSerializer = ApiGear::ObjectLink::NlohmannSerializer;

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
        client()->setRemoteProperty("demo.Calc/total", ContentSerializer::Value::serialize(value));
    }
    int add(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            std::cout << "invoke reply" << arg.methodId << ApiGear::ObjectLink::getAsString(arg.value);
        };
        client()->invokeRemote("demo.Calc/add", ContentSerializer::Arguments::serialize(a), func);

        return -1;
    }
    int sub(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            std::cout << "invoke reply " << arg.methodId << ApiGear::ObjectLink::getAsString(arg.value);
        };
        client()->invokeRemote("demo.Calc/sub", ContentSerializer::Arguments::serialize(a), func);
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
        std::cout << "onSignal" << name  << ApiGear::ObjectLink::getAsString(args) << std::endl;
        ContentSerializer::Arguments::Deserializer reader(args);
        int signalArgValue;
        reader.getNext(signalArgValue);
        events.push_back(std::make_pair(name, signalArgValue));

    }
    void olinkOnPropertyChanged(const std::string& name, const OLinkContent& value) override {
        std::cout << "onPropertyChanged" << name << ApiGear::ObjectLink::getAsString(value) << std::endl;
        std::string path = Name::getMemberName(name);
        if(path == "total") {
            int total = 0;
            ContentSerializer::Value::deserialize(value, total);
            if(m_total != total) {
                m_total = total;
            }
        }

    }
    void olinkOnInit(const std::string& name, const OLinkContent& props, IClientNode* client) override {
        std::cout << "CalcSink.olinkOnInit: " << name << ApiGear::ObjectLink::getAsString(props) << std::endl;
        m_client = client;
        m_ready = true;
        InitialProperty expectTotalProperty;
        ContentSerializer::Arguments::Deserializer reader(props);
        reader.getNext(expectTotalProperty);
        if(expectTotalProperty.propertyName == "total") {
            int total = 0;
            ContentSerializer::fromInitialProperty(expectTotalProperty, total);
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
