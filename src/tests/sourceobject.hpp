#pragma once

#include "olink/sourcetypes.h"
#include "olink/core/protocol.h"
#include <iostream>

using namespace ApiGear::ObjectLink;

class CalcSource: public IObjectSource {
public:
    CalcSource()
        : m_service(nullptr)
        , m_total(1)
    {
    }
    virtual ~CalcSource() override {}

    ISourceLink* service() const {
        assert(m_service);
        return m_service;
    }

    int add(int value) {
        m_total += value;
        service()->notifyPropertyChange("demo.Calc/total", m_total);
        m_events.push_back({ "demo.Calc/add", value });
        m_events.push_back({ "demo.Calc/total", m_total });
        if(m_total >= 10) {
            service()->notifySignal("demo.Calc/hitUpper", { 10 });
            m_events.push_back({ "demo.Calc/hitUpper", 10 });
        }
        return m_total;
    }

    int sub(int value) {
        m_total -= value;
        service()->notifyPropertyChange("demo.Calc/total", m_total);
        m_events.push_back({ "demo.Calc/sub", value });
        m_events.push_back({ "demo.Calc/total", m_total });
        if(m_total <= 0) {
            service()->notifySignal("demo.Calc/hitLower", { 0 });
            m_events.push_back({ "demo.Calc/hitLower", 0 });
        }
        return m_total;
    }
    void notifyShutdown(int timeout) {
        service()->notifySignal("demo.Calc/timeout", { timeout });
    }
    // IServiceObjectListener interface
public:
    std::string getObjectName() override {
        return "demo.Calc";
    }
    json invoke(std::string name, json args) override {
        std::cout << "invoke" << name << args.dump();
        std::string path = Name::pathFromName(name);
        if(path == "add") {
            int a = args[0].get<int>();
            int result = add(a);
            return result;
        }
        return json();
    }
    void setProperty(std::string name, json value) override {
        std::cout << "setProperty" << name << value.dump();
        std::string path = Name::pathFromName(name);
        if(path == "total") {
            int total = value.get<int>();
            if(m_total != total) {
                m_total = total;
                service()->notifyPropertyChange(name, total);
            }
        }
    }
    void linked(std::string name, ISourceLink *service) override {
        std::cout << "linked" << name;
        m_service = service;
    }
    void unlinked(std::string name) override
    {
        std::cout << "unlinked" << name;
        m_service = nullptr;
    }
    json collectProperties() override
    {
        return {{ "total", m_total }};
    }
private:
    ISourceLink* m_service;
    int m_total;
    std::vector<json> m_events;
};
