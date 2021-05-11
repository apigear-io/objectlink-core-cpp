#pragma once

#include "olink/sinktypes.h"
#include "spdlog/spdlog.h"

using namespace ApiGear::ObjectLink;

class CalcSink: public IObjectLinkSink {
public:
    CalcSink(IObjectLinkClient *m_client)
        : m_client(m_client)
        , m_total(0)
        , m_isReady(false)
    {
    }
    virtual ~CalcSink() override {}
    int total() const {
        return m_total;
    }
    void setTotal(int value) {
        m_client->setProperty("demo.Calc/total", value);
    }
    int add(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            spdlog::info("invoke reply name:{} value:{}", arg.name, arg.value.dump());
        };
        m_client->invoke("demo.Calc/add", { a }, func);

        return -1;
    }
    int sub(int a) {
        InvokeReplyFunc func = [](InvokeReplyArg arg) {
            spdlog::info("invoke reply name:{} value:{}", arg.name, arg.value.dump());
        };
        m_client->invoke("demo.Calc/sub", { a }, func);
        return -1;
    }
    bool isReady() const {
        return m_isReady;
    }
    // IClientObjectHandler interface
public:
    void onSignal(std::string name, json args) override {
        SPDLOG_TRACE(name, args.dump());
        events.push_back({name, args});

    }
    void onPropertyChanged(std::string name, json value) override {
        SPDLOG_TRACE(name, value.dump());
        std::string path = Name::pathFromName(name);
        if(path == "total") {
            int total = value.get<int>();
            if(m_total != total) {
                m_total = total;
            }
        }

    }
    void onInit(std::string name, json props) override {
        SPDLOG_TRACE("on init {} {}", name, props.dump());
        m_isReady = true;
        if(props.contains("total")) {
            int total = props["total"].get<int>();
            if(m_total != total) {
                m_total = total;
            }
        }
    }
public:
    std::list<json> events;
private:
    IObjectLinkClient *m_client;
    int m_total;
    bool m_isReady;

};
