#pragma once

#include <QtCore>

#include "olink/clientnode.h"
#include "nlohmann/json.hpp"
#include "olink/clientregistry.h"
#include "olink/iobjectsink.h"

using json = nlohmann::json;

using namespace ApiGear::ObjectLink;

class QClientRegistry: public QObject, public ApiGear::ObjectLink::ClientRegistry
{
    Q_OBJECT
    QClientRegistry() {};
public:
    static QClientRegistry& getInstance(){
        static QClientRegistry r;
        return r;
    };
};

class CalcSink
    : public QObject
    , public IObjectSink
{
    Q_OBJECT
    Q_PROPERTY(int total READ total WRITE setTotal NOTIFY totalChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY readyChanged)
public:
    CalcSink(QObject *parent=nullptr);
    virtual ~CalcSink();
    Q_INVOKABLE void add(int a);
    Q_INVOKABLE void sub(int a);
    Q_INVOKABLE void clear();

    int total() const;
    void setTotal(int total);
    bool isReady() const;
signals:
    void totalChanged(int a);
    void readyChanged(bool ready);
    void maxReached(int value);
    void minReached(int value);
public: // IObjectLinkSink
    virtual std::string olinkObjectName() override;
    virtual void olinkOnSignal(const std::string& name, const json& args) override;
    virtual void olinkOnPropertyChanged(const std::string& name, const json& value) override;
    virtual void olinkOnInit(const std::string& name, const json& props, IClientNode *node) override;
    virtual void olinkOnRelease() override;
private:
    int m_total;
    bool m_ready;
    std::string m_name;
    IClientNode *m_node;
};
