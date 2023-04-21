

#include "api/generated/api/datastructs.api.h"
#include "api/generated/olink/testapiservice.h"
#include "api/generated/core/api.json.adapter.h"

#include "olink/iremotenode.h"
#include "olink/remoteregistry.h"
#include "olink/core/types.h"
#include "apigear/utilities/logger.h"

#include <iostream>


using namespace Test::Api;
using namespace Test::Api::olink;

namespace 
{
const std::string interfaceId = "api.TestApi";
}

TestApiService::TestApiService(std::shared_ptr<ITestApi> TestApi, ApiGear::ObjectLink::RemoteRegistry& registry)
    : m_TestApi(TestApi)
    , m_registry(registry)
{
    m_TestApi->_getPublisher().subscribeToAllChanges(*this);
}

TestApiService::~TestApiService()
{
    m_TestApi->_getPublisher().unsubscribeFromAllChanges(*this);
}

std::string TestApiService::olinkObjectName() {
    return interfaceId;
}

nlohmann::json TestApiService::olinkInvoke(const std::string& methodId, const nlohmann::json& fcnArgs) {
    AG_LOG_DEBUG("TestApiService invoke " + methodId);
    const auto& memberMethod = ApiGear::ObjectLink::Name::getMemberName(methodId);
    if(memberMethod == "funcInt") {
        const int& paramInt = fcnArgs.at(0);
        int result = m_TestApi->funcInt(paramInt);
        return result;
    }
    if(memberMethod == "funcFloat") {
        const float& paramFloat = fcnArgs.at(0);
        float result = m_TestApi->funcFloat(paramFloat);
        return result;
    }
    if(memberMethod == "funcString") {
        const std::string& paramString = fcnArgs.at(0);
        std::string result = m_TestApi->funcString(paramString);
        return result;
    }
    return nlohmann::json();
}

void TestApiService::olinkSetProperty(const std::string& propertyId, const nlohmann::json& value) {
    AG_LOG_DEBUG("TestApiService set property " + propertyId);
    const auto& memberProperty = ApiGear::ObjectLink::Name::getMemberName(propertyId);
    if(memberProperty == "propInt") {
        int propInt = value.get<int>();
        m_TestApi->setPropInt(propInt);
    }
    if(memberProperty == "propFloat") {
        float propFloat = value.get<float>();
        m_TestApi->setPropFloat(propFloat);
    }
    if(memberProperty == "propString") {
        std::string propString = value.get<std::string>();
        m_TestApi->setPropString(propString);
    } 
}

void TestApiService::olinkLinked(const std::string& objectId, ApiGear::ObjectLink::IRemoteNode* /*node*/) {
    AG_LOG_DEBUG("TestApiService linked " + objectId);
}

void TestApiService::olinkUnlinked(const std::string& objectId){
    AG_LOG_DEBUG("TestApiService unlinked " + objectId);
}

nlohmann::json TestApiService::olinkCollectProperties()
{
    return nlohmann::json::object({
        { "propInt", m_TestApi->getPropInt() },
        { "propFloat", m_TestApi->getPropFloat() },
        { "propString", m_TestApi->getPropString() }
    });
}
void TestApiService::onSigInt(int paramInt)
{
    const nlohmann::json args = { paramInt };
    static const auto& signalId = ApiGear::ObjectLink::Name::createMemberId(olinkObjectName(), "sigInt");
    static const auto objectId = ApiGear::ObjectLink::Name::getObjectId(signalId);
    for(auto node: m_registry.getNodes(objectId)) {
        auto lockedNode = node.lock();
        if(lockedNode) {
            lockedNode->notifySignal(signalId, args);
        }
    }
}
void TestApiService::onSigFloat(float paramFloat)
{
    const nlohmann::json args = { paramFloat };
    static const auto& signalId = ApiGear::ObjectLink::Name::createMemberId(olinkObjectName(), "sigFloat");
    static const auto objectId = ApiGear::ObjectLink::Name::getObjectId(signalId);
    for(auto node: m_registry.getNodes(objectId)) {
        auto lockedNode = node.lock();
        if(lockedNode) {
            lockedNode->notifySignal(signalId, args);
        }
    }
}
void TestApiService::onSigString(const std::string& paramString)
{
    const nlohmann::json args = { paramString };
    static const auto& signalId = ApiGear::ObjectLink::Name::createMemberId(olinkObjectName(), "sigString");
    static const auto objectId = ApiGear::ObjectLink::Name::getObjectId(signalId);
    for(auto node: m_registry.getNodes(objectId)) {
        auto lockedNode = node.lock();
        if(lockedNode) {
            lockedNode->notifySignal(signalId, args);
        }
    }
}
void TestApiService::onPropIntChanged(int propInt)
{
    static const auto propertyId = ApiGear::ObjectLink::Name::createMemberId(olinkObjectName(), "propInt");
    static const auto objectId = ApiGear::ObjectLink::Name::getObjectId(propertyId);
    for(auto node: m_registry.getNodes(objectId)) {
        auto lockedNode = node.lock();
        if(lockedNode) {
            lockedNode->notifyPropertyChange(propertyId, propInt);
        }
    }
}
void TestApiService::onPropFloatChanged(float propFloat)
{
    static const auto& propertyId = ApiGear::ObjectLink::Name::createMemberId(olinkObjectName(), "propFloat");
    static const auto objectId = ApiGear::ObjectLink::Name::getObjectId(propertyId);
    for(auto node: m_registry.getNodes(objectId)) {
        auto lockedNode = node.lock();
        if(lockedNode) {
            lockedNode->notifyPropertyChange(propertyId, propFloat);
        }
    }
}
void TestApiService::onPropStringChanged(const std::string& propString)
{
    static const auto& propertyId = ApiGear::ObjectLink::Name::createMemberId(olinkObjectName(), "propString");
    static const auto objectId = ApiGear::ObjectLink::Name::getObjectId(propertyId);
    for(auto node: m_registry.getNodes(objectId)) {
        auto lockedNode = node.lock();
        if(lockedNode) {
            lockedNode->notifyPropertyChange(propertyId, propString);
        }
    }
}

