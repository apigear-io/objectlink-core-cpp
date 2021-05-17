#pragma once

#include <QtCore>

#include "olink/sinktypes.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

using namespace ApiGear::ObjectLink;

class CalcSink
    : public QObject
    , public ISink
{
    Q_OBJECT
    Q_PROPERTY(int total READ total WRITE setTotal NOTIFY totalChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY readyChanged)
public:
    CalcSink(QObject *parent=nullptr);
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
    virtual void onSignal(std::string name, json args) override;
    virtual void onPropertyChanged(std::string name, json value) override;
    virtual void onInit(std::string name, json props, IClient *client) override;
    virtual void onRelease() override;
private:
    int m_total;
    bool m_ready;
    std::string m_name;
    IClient *m_client;
};
