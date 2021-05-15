#pragma once
#include "olink/sourcetypes.h"

using namespace ApiGear::ObjectLink;

class CalcSource: public IObjectLinkSource {
public:
    CalcSource();
    virtual ~CalcSource() override;

    IService *service() const;

    int add(int value);

    int sub(int value);
    void notifyShutdown(int timeout);
    // IServiceObjectListener interface
public:
    std::string getObjectName() override;
    json invoke(std::string name, json args) override;
    void setProperty(std::string name, json value) override;
    void linked(std::string name, IService *service) override;
    void unlinked(std::string name) override;
    json collectProperties() override;
private:
    IService* m_service;
    int m_total;
    std::vector<json> m_events;
};

