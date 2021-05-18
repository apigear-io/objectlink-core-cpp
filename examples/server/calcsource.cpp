#include "calcsource.h"

#include <iostream>
using namespace ApiGear::ObjectLink;


CalcSource::CalcSource()
    : m_link(nullptr)
    , m_total(1)
{
}

CalcSource::~CalcSource() {}

IObjectSourceNode *CalcSource::link() const {
    assert(m_link);
    return m_link;
}

int CalcSource::add(int value) {
    std::cout << "add: " + std::to_string(value) << std::endl;
    m_total += value;
    link()->notifyPropertyChange("demo.Calc/total", m_total);
    if(m_total >= 10) {
        link()->notifySignal("demo.Calc/hitUpper", { 10 });
    }
    return m_total;
}

int CalcSource::sub(int value) {
    std::cout << "sub: " + std::to_string(value) << std::endl;
    m_total -= value;
    link()->notifyPropertyChange("demo.Calc/total", m_total);
    if(m_total <= 0) {
        link()->notifySignal("demo.Calc/hitLower", { 0 });
    }
    return m_total;
}

void CalcSource::clear()
{
    m_total = 0;
    link()->notifyPropertyChange("demo.Calc/total", m_total);
}

void CalcSource::notifyShutdown(int timeout) {
    link()->notifySignal("demo.Calc/timeout", { timeout });
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
    } else if(path == "sub") {
        int a = args[0].get<int>();
        int result = sub(a);
        return result;
    } else if(path == "clear") {
        clear();
        return json{};
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
            link()->notifyPropertyChange(name, total);
        }
    }
}

void CalcSource::linked(std::string name, IObjectSourceNode *link) {
    std::cout << "linked" << name;
    m_link = link;
}

void CalcSource::unlinked(std::string name)
{
    std::cout << "unlinked" << name;
    m_link = nullptr;
}

json CalcSource::collectProperties()
{
    return {{ "total", m_total }};
}
