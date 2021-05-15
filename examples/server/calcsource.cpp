#include "calcsource.h"

#include <iostream>
using namespace ApiGear::ObjectLink;


CalcSource::CalcSource()
    : m_service(nullptr)
    , m_total(1)
{
}

CalcSource::~CalcSource() {}

IService *CalcSource::service() const {
    assert(m_service);
    return m_service;
}

int CalcSource::add(int value) {
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

int CalcSource::sub(int value) {
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

void CalcSource::notifyShutdown(int timeout) {
    service()->notifySignal("demo.Calc/timeout", { timeout });
}

std::string CalcSource::getObjectName() {
    return "demo.Calc";
}

json CalcSource::invoke(std::string name, json args) {
    std::cout << "invoke" << name << args.dump();
    std::string path = Name::pathFromName(name);
    if(path == "add") {
        int a = args[0].get<int>();
        int result = add(a);
        return result;
    }
    return json();
}

void CalcSource::setProperty(std::string name, json value) {
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

void CalcSource::linked(std::string name, IService *service) {
    std::cout << "linked" << name;
    m_service = service;
}

void CalcSource::unlinked(std::string name)
{
    std::cout << "unlinked" << name;
    m_service = nullptr;
}

json CalcSource::collectProperties()
{
    return {{ "total", m_total }};
}
