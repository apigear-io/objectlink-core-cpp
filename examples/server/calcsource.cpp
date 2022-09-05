#include "calcsource.h"

#include <iostream>
using namespace ApiGear::ObjectLink;


CalcSource::CalcSource(RemoteRegistry& registry)
    : m_node(nullptr)
    , m_total(1)
    , m_registry(&registry)
{
    m_registry->addSinkSource(this);
}

CalcSource::~CalcSource() {
    m_registry->removeSinkSource(this);
}

IRemoteNode *CalcSource::node() const {
    assert(m_node);
    return m_node;
}

int CalcSource::add(int value) {
    std::cout << "add: " + std::to_string(value) << std::endl;
    m_total += value;
    node()->notifyPropertyChange("demo.Calc/total", m_total);
    if(m_total >= 10) {
        node()->notifySignal("demo.Calc/hitUpper", { 10 });
    }
    return m_total;
}

int CalcSource::sub(int value) {
    std::cout << "sub: " + std::to_string(value) << std::endl;
    m_total -= value;
    node()->notifyPropertyChange("demo.Calc/total", m_total);
    if(m_total <= 0) {
        node()->notifySignal("demo.Calc/hitLower", { 0 });
    }
    return m_total;
}

void CalcSource::clear()
{
    m_total = 0;
    node()->notifyPropertyChange("demo.Calc/total", m_total);
}

void CalcSource::notifyShutdown(int timeout) {
    node()->notifySignal("demo.Calc/timeout", { timeout });
}

std::string CalcSource::olinkObjectName() {
    return "demo.Calc";
}

nlohmann::json CalcSource::olinkInvoke(std::string name, nlohmann::json args) {
    std::cout << "invoke" << name << args.dump();
    std::string path = Name::getMemberName(name);
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
        return nlohmann::json{};
    }
    return nlohmann::json();
}

void CalcSource::olinkSetProperty(std::string name, nlohmann::json value) {
    std::cout << "setProperty" << name << value.dump();
    std::string path = Name::getMemberName(name);
    if(path == "total") {
        int total = value.get<int>();
        if(m_total != total) {
            m_total = total;
            node()->notifyPropertyChange(name, total);
        }
    }
}

void CalcSource::olinkLinked(std::string name, IRemoteNode *node) {
    std::cout << "linked" << name;
    m_node = node;
}

void CalcSource::olinkUnlinked(std::string name)
{
    std::cout << "unlinked" << name;
    m_node = nullptr;
}

nlohmann::json CalcSource::olinkCollectProperties()
{
    return {{ "total", m_total }};
}
