#include "olink/iobjectsource.h"
#include "olink/remotenode.h"
#include "api/generated/olink/testapiservice.h"
#include "api/implementation/testapi.h"
#include "olink/remoteregistry.h"
#include "olink/core/types.h"
#include <memory>
#include <chrono>
#include <iostream>
#include <algorithm>

namespace {

    // Converter used in tests, should be same as one used by node.
    ApiGear::ObjectLink::MessageConverter converter(ApiGear::ObjectLink::MessageFormat::JSON);
}


class TestService : public Test::Api::olink::TestApiService
{
public:
    TestService(std::string name, std::shared_ptr<Test::Api::ITestApi> TestApi, ApiGear::ObjectLink::RemoteRegistry& registry)
        :TestApiService(TestApi, registry),
        m_name(name)
    {}

    std::string olinkObjectName() override {
        return m_name;
    }
    std::string m_name;
};

/*
* Test to check bottle necks for the remote registry
* Test attaches source_number - adds them to registry and sends the link message to tested remote node
* Then for each source a messages_number is sent with changing the int property to value of current number of a message to sent
* Then for each source the unregister message is sent to tested remote node.
*/
int main()
{
    auto repeat_test_count = 1u;
    std::vector<uint16_t> time;
    auto source_number = 500u;
    auto messages_number = 1000u;

    for (auto count = 0u; count < repeat_test_count; count++)
    {
        std::vector<std::shared_ptr<Test::Api::TestApi>> sources;
        std::vector<std::shared_ptr<TestService>> services;
        std::map<std::string, nlohmann::json> linkMessages;
        std::map<std::string, nlohmann::json> unlinkMessages;
        ApiGear::ObjectLink::RemoteRegistry registry;
        //prepare a service and all it requires for test:
        // source
        // link message which will be sent during test
        // unlink message which will be sent during test
        // put a source to registry
        for (auto i = 0u; i < source_number; i++)
        {
            auto source = std::make_shared<Test::Api::TestApi>();
            auto sourceService = std::make_shared<TestService>("Source" + std::to_string(i), source, registry);
            auto linkMessage = ApiGear::ObjectLink::Protocol::linkMessage(sourceService->olinkObjectName());
            auto unlinkMessage = ApiGear::ObjectLink::Protocol::unlinkMessage(sourceService->olinkObjectName());
            linkMessages[sourceService->olinkObjectName()] = converter.toString(linkMessage);
            unlinkMessages[sourceService->olinkObjectName()] = converter.toString(unlinkMessage);
            services.push_back(sourceService);
            sources.push_back(source);
            registry.addSource(sourceService);
        }

        // Prepare test remote node, with onWrite that does nothing.
        auto testedNode = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
        testedNode->onWrite([](const auto& /*msg*/){/*do nothing with the sent message*/ });

        auto begin = std::chrono::high_resolution_clock::now();
        {
            for (auto source : services)
            {
                testedNode->handleMessage(linkMessages.at(source->olinkObjectName()));
            }

            for (auto source : sources)
            {
                for (auto i = 0u; i < messages_number; i++)
                {
                    source->setPropInt(i);
                }
            }
            for (auto source : services)
            {
                testedNode->handleMessage(unlinkMessages.at(source->olinkObjectName()));
            }
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        time.push_back(elapsed.count());
    }

    std::cout << "average time measured " << std::accumulate(time.begin(), time.end(), 0.0) / time.size() << std::endl;
    auto max = std::max_element(time.begin(), time.end());
    auto min = std::min_element(time.begin(), time.end());
    std::cout << "lowest time measured " << *min << std::endl;
    std::cout << "highest time measured " << *max << std::endl;

}
