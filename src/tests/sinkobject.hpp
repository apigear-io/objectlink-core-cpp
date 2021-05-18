#pragma once

#include "olink/sink/sinktypes.h"
#include <iostream>

using namespace ApiGear::ObjectLink;

class CalcSink: public IObjectSink {
public:
    CalcSink()
        : m_client(nullptr)
        , m_total(0)
        , m_ready(false)
    {
    }
    virtual ~CalcSink() override {}
    int total() const {
        return m_total;
    }
    void setTotal(int value) {
        client()->setProperty("demo.Calc/total", value);
    }
    int add(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            std::cout << "invoke reply" << arg.name << arg.value.dump();
        };
        client()->invoke("demo.Calc/add", { a }, func);

        return -1;
    }
    int sub(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            std::cout << "invoke reply " << arg.name << arg.value.dump();
        };
        client()->invoke("demo.Calc/sub", { a }, func);
        return -1;
    }
    IObjectSinkNode *client() const {
        assert(m_client);
        return m_client;
    }
    bool isReady() const {
        return m_ready;
    }
    // IClientObjectHandler interface
public:
    void onSignal(std::string name, json args) override {
        std::cout << "onSignal" << name  << args.dump() << std::endl;
        events.push_back({name, args});

    }
    void onPropertyChanged(std::string name, json value) override {
        std::cout << "onPropertyChanged" << name << value.dump() << std::endl;
        std::string path = Name::pathFromName(name);
        if(path == "total") {
            int total = value.get<int>();
            if(m_total != total) {
                m_total = total;
            }
        }

    }
    void onInit(std::string name, json props, IObjectSinkNode *client) override {
        std::cout << "onInit" << name << props.dump() << std::endl;
        m_client = client;
        m_ready = true;
        if(props.contains("total")) {
            int total = props["total"].get<int>();
            if(m_total != total) {
                m_total = total;
            }
        }
    }
    void onRelease() override {
        m_ready = false;
        m_client = nullptr;
    }
public:
    std::list<json> events;
private:
    IObjectSinkNode *m_client;
    int m_total;
    bool m_ready;

};
