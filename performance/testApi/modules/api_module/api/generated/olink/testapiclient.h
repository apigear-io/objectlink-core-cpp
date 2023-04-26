
#pragma once

#include "api/generated/api/common.h"
#include "api/generated/api/api.h"
#include "api/generated/core/testapi.data.h"

#include "olink/iobjectsink.h"

#include <future>
#include <memory>

namespace ApiGear{
namespace ObjectLink{
class IClientNode;
}
}

namespace Test {
namespace Api {
namespace olink {
/**
* Adapts the general OLink Client handler to a TestApi publisher in a way it provides access 
* to remote TestApi services. 
* Sends and receives data over the network with ObjectLink protocol, through the communication node. 
* see https://objectlinkprotocol.net for ObjectLink details.
* see https://github.com/apigear-io/objectlink-core-cpp.git for olink client node - abstraction over the network.
* see Apigear::ObjectLink::OLinkConnection for Olink Client Handler implementation.
*     It provides a network implementation and tools to connect TestApiClient to it.
* Use on client side to request changes of the TestApi on the server side 
* and to subscribe for the TestApi changes.
*/
class TEST_API_EXPORT TestApiClient : public ITestApi,
    public ApiGear::ObjectLink::IObjectSink
{
public:

    /** ctor */
    explicit TestApiClient();
    /** dtor */
    virtual ~TestApiClient() = default;
    /**
    * Property getter
    * @return Locally stored locally value for PropInt.
    */
    int getPropInt() const override;
    /**
    * Request setting a property on the TestApi service.
    * @param The value to which set request is send for the PropInt.
    */
    void setPropInt(int propInt) override;
    /**
    * Property getter
    * @return Locally stored locally value for PropFloat.
    */
    float getPropFloat() const override;
    /**
    * Request setting a property on the TestApi service.
    * @param The value to which set request is send for the PropFloat.
    */
    void setPropFloat(float propFloat) override;
    /**
    * Property getter
    * @return Locally stored locally value for PropString.
    */
    const std::string& getPropString() const override;
    /**
    * Request setting a property on the TestApi service.
    * @param The value to which set request is send for the PropString.
    */
    void setPropString(const std::string& propString) override;
    /**
    * Remote call of ITestApi::funcInt on the TestApi service.
    * Uses funcIntAsync
    */
    int funcInt(int paramInt) override;
    /**
    * Remote call of ITestApi::funcInt on the TestApi service.
    */
    std::future<int> funcIntAsync(int paramInt) override;
    /**
    * Remote call of ITestApi::funcFloat on the TestApi service.
    * Uses funcFloatAsync
    */
    float funcFloat(float paramFloat) override;
    /**
    * Remote call of ITestApi::funcFloat on the TestApi service.
    */
    std::future<float> funcFloatAsync(float paramFloat) override;
    /**
    * Remote call of ITestApi::funcString on the TestApi service.
    * Uses funcStringAsync
    */
    std::string funcString(const std::string& paramString) override;
    /**
    * Remote call of ITestApi::funcString on the TestApi service.
    */
    std::future<std::string> funcStringAsync(const std::string& paramString) override;

    /** The publisher to subscribe to. */
    ITestApiPublisher& _getPublisher() const override;
    
    /**
    * Informs if the TestApiClient is ready to send and receive messages.
    * @return true if TestApi is operable, false otherwise.
    */
    bool isReady() const;

    /**
    * The name of the object for which this sink is created, object on server side has to have the same name.
    * It serves as an identifier for the client registry, it has to be unique for the pair sink object - client node.
    * Passed in the olink messages as an object identifier.
    */
    std::string olinkObjectName() override;
    
    /**
    * Information about signal emission on a server side to all subscribers.
    * @param signalId Unique identifier for the signal emitted from object.
    * @param args The arguments for the signal.
    */
    void olinkOnSignal(const std::string& signalId, const nlohmann::json& args) override;
    
    /**
    * Applies the information about the property changed on server side.
    * @param propertyId Unique identifier of a changed property in object .
    * @param value The value of the property.
    */
    void olinkOnPropertyChanged(const std::string& propertyId, const nlohmann::json& value) override;
    
    /** Informs this object sink that connection was is established.
    * @param interfaceId The name of the object for which link was established.
    * @param props Initial values obtained from the TestApi service
    * @param the initialized link endpoint for this sink.
    */
    void olinkOnInit(const std::string& interfaceId, const nlohmann::json& props, ApiGear::ObjectLink::IClientNode *node) override;
    /**
    * Informs this object source that the link was disconnected and cannot be used anymore.
    */
    void olinkOnRelease() override;

private:
    /**
    * Applies received data to local state and publishes changes to subscribers.
    * @param the data received from TestApi service.
    */
    void applyState(const nlohmann::json& fields);
    /**  Updates local value for PropInt and informs subscriber about the change*/
    void setPropIntLocal(int propInt);
    /**  Updates local value for PropFloat and informs subscriber about the change*/
    void setPropFloatLocal(float propFloat);
    /**  Updates local value for PropString and informs subscriber about the change*/
    void setPropStringLocal(const std::string& propString);

    /** Local storage for properties values. */
    TestApiData m_data;

    /** 
    * An abstraction layer over the connection with service for the TestApiClient.
    * Handles incoming and outgoing messages.
    * Is given when object is linked with the service.
    */
    ApiGear::ObjectLink::IClientNode* m_node = nullptr;

    /** The publisher for TestApi */
    std::unique_ptr<ITestApiPublisher> m_publisher;
};
} // namespace olink
} // namespace Api
} // namespace Test
