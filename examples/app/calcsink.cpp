#include "calcsink.h"

#include "olink/core/types.h"
#include "olink/sinknode.h"
#include "qclientio.h"

using namespace ApiGear::ObjectLink;

CalcSink::CalcSink(QObject *parent)
    : QObject(parent)
    , m_link(nullptr)
{
    SinkNode* node = SinkNodeManager::get().getSinkNode("client1");
    if(node) {
        node->addObjectSink("demo.Calc", this);
    }
//    Q_ASSERT(m_client);
//    m_client->link("demo.Calc");
}

void CalcSink::add(int a)
{
    assert(m_link);
    InvokeReplyFunc func = [this](InvokeReplyArg arg) {};
    m_link->invoke("demo.Calc/add", { a }, func);
}

void CalcSink::sub(int a)
{
    assert(m_link);
    InvokeReplyFunc func = [this](InvokeReplyArg arg) {};
    m_link->invoke("demo.Calc/sub", { a }, func);
}

void CalcSink::clear()
{
    assert(m_link);
    m_link->invoke("demo.Calc/clear");
}

int CalcSink::total() const {
    return m_total;
}

void CalcSink::setTotal(int total)
{
    assert(m_link);
    m_link->setProperty("demo.Calc/total", total);
}

bool CalcSink::isReady() const {
    return m_ready;
}


void CalcSink::onSignal(std::string name, json args)
{
    std::string path = Name::pathFromName(name);
    if(path == "maxReached") {
        int value = args[0].get<int>();
        emit maxReached(value);
    }
    else if(path == "minReached") {
        int value = args[0].get<int>();
        emit minReached(value);
    }
}
void CalcSink::onPropertyChanged(std::string name, json value)
{
    std::string path = Name::pathFromName(name);
    if(path == "total") {
        int total = value.get<int>();
        if(m_total != total) {
            m_total = total;
            emit totalChanged(total);
        }
    }
}
void CalcSink::onInit(std::string name, json props, ISinkLink *link)
{
    m_name = name;
    m_ready = true;
    m_link = link;
    if(props.contains("total")) {
        int total = props["total"].get<int>();
        if(m_total != total) {
            m_total = total;
            emit totalChanged(total);
        }
    }
    assert(m_link);
}

void CalcSink::onRelease()
{
    m_ready = false;
    m_link = nullptr;
}
