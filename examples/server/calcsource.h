#pragma once
#include "olink/source/sourcetypes.h"

using namespace ApiGear::ObjectLink;

class CalcSource: public IObjectSource {
public:
    CalcSource();
    virtual ~CalcSource() override;

    IObjectSourceNode *link() const;

    int add(int value);

    int sub(int value);
    void clear();
    void notifyShutdown(int timeout);
    // IServiceObjectListener interface
public:
    std::string getObjectName() override;
    json invoke(std::string name, json args) override;
    void setProperty(std::string name, json value) override;
    void linked(std::string name, IObjectSourceNode *service) override;
    void unlinked(std::string name) override;
    json collectProperties() override;
private:
    IObjectSourceNode* m_link;
    int m_total;
};

