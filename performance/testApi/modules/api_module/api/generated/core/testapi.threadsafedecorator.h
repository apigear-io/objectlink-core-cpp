
#pragma once
#include "api/generated/api/api.h"
#include "api/generated/api/common.h"
#include <memory>
#include <shared_mutex>

namespace Test {
namespace Api {

/** 
* @brief The TestApiThreadSafeDecorator can be used to make property access thread safe.
*
* Each property is guarded with its own mutex.
* Multiple read/get operations can occur at the same time but only one write/set operation at a time.
*
* Operations are not guarded by default since the function logic can be too complex than to simply lock it.
* However, functions can be locked by just adding the same mechanism in the implementation file of
* the TestApi interface.
* @see TestApi
*
\code{.cpp}
using namespace Test::Api;

std::unique_ptr<ITestApi> testTestApi = std::make_unique<TestApiThreadSafeDecorator>(std::make_shared<TestApi>());

// Thread safe access
auto propInt = testTestApi->getPropInt();
testTestApi->setPropInt(0);
auto propFloat = testTestApi->getPropFloat();
testTestApi->setPropFloat(0.0f);
auto propString = testTestApi->getPropString();
testTestApi->setPropString(std::string());
\endcode
*/
class TEST_API_EXPORT TestApiThreadSafeDecorator : public ITestApi
{
public:
    /** 
    * ctor
    * @param impl The TestApi object to make thread safe.
    */
    explicit TestApiThreadSafeDecorator(std::shared_ptr<ITestApi> impl);

    /** 
    * Forwards call to TestApi implementation.
    * @warning This forward call is not made thread safe by this class.
    */
    int funcInt(int paramInt) override;
    /** 
    * Forwards call to TestApi implementation.
    * @warning This forward call is not made thread safe by this class.
    */
    std::future<int> funcIntAsync(int paramInt) override;

    /** 
    * Forwards call to TestApi implementation.
    * @warning This forward call is not made thread safe by this class.
    */
    float funcFloat(float paramFloat) override;
    /** 
    * Forwards call to TestApi implementation.
    * @warning This forward call is not made thread safe by this class.
    */
    std::future<float> funcFloatAsync(float paramFloat) override;

    /** 
    * Forwards call to TestApi implementation.
    * @warning This forward call is not made thread safe by this class.
    */
    std::string funcString(const std::string& paramString) override;
    /** 
    * Forwards call to TestApi implementation.
    * @warning This forward call is not made thread safe by this class.
    */
    std::future<std::string> funcStringAsync(const std::string& paramString) override;

    /** Guards and forwards call to TestApi implementation. */
    void setPropInt(int propInt) override;
    /** Guards and forwards call to TestApi implementation. */
    int getPropInt() const override;

    /** Guards and forwards call to TestApi implementation. */
    void setPropFloat(float propFloat) override;
    /** Guards and forwards call to TestApi implementation. */
    float getPropFloat() const override;

    /** Guards and forwards call to TestApi implementation. */
    void setPropString(const std::string& propString) override;
    /** Guards and forwards call to TestApi implementation. */
    const std::string& getPropString() const override;

    /**
    * Access to a publisher, use it to subscribe for TestApi changes and signal emission.
    * This call is thread safe.
    * @return The publisher for TestApi.
    */
    ITestApiPublisher& _getPublisher() const override;
private:
    /** The TestApi object which is guarded */
    std::shared_ptr<ITestApi> m_impl;
    // Mutex for propInt property
    mutable std::shared_timed_mutex m_propIntMutex;
    // Mutex for propFloat property
    mutable std::shared_timed_mutex m_propFloatMutex;
    // Mutex for propString property
    mutable std::shared_timed_mutex m_propStringMutex;
};
} // namespace Api
} // namespace Test
