#include "api/generated/olink/testapiclient.h"
#include "olink/clientnode.h"
#include "olink/clientregistry.h"
#include "olink/core/types.h"
#include <memory>
#include <chrono>
#include <iostream>
#include <algorithm>


class TestSink : public Test::Api::olink::TestApiClient
{
public:
    TestSink(std::string name)
    :m_name(name)
    {}

    std::string olinkObjectName() override {return m_name;}
    std::string m_name;
};

namespace {
    ApiGear::ObjectLink::MessageConverter converter(ApiGear::ObjectLink::MessageFormat::JSON);
}


/*
* Test to check bottle necks for the client registry
* Test attaches sink_number of sinks - adds them to registry and requests linking for them.
* Then the init message is sent to test client node.
* Then for each sink a messages_number number of int property change requests are sent.
* At the end of test all of the sinks are unlinked.
* The write function is set to do nothing. Normally the network layer sets here a handle to send the messages.
*/
int main()
{
    std::vector<uint16_t> time;
    auto repeat_test_count = 10u;
    auto sink_number = 500u;
    auto messages_number = 1000u;

    // Prepare sinks and init messages for them.
    std::vector<std::shared_ptr<TestSink>> sinks;
    nlohmann::json initProperties = {};
    std::map<std::string, nlohmann::json> initMessages;
    for (auto i = 0u; i < sink_number; i++)
    {
        auto sink = std::make_shared<TestSink>("Sink" + std::to_string(i));
        auto InitMessage = ApiGear::ObjectLink::Protocol::initMessage(sink->olinkObjectName(), {});
        initMessages[sink->olinkObjectName()] = converter.toString(InitMessage);
        sinks.push_back(sink);
    }


    // start test
    for (auto count = 0u; count < repeat_test_count; count++)
    {
        ApiGear::ObjectLink::ClientRegistry registry;
        auto testedNode = ApiGear::ObjectLink::ClientNode::create(registry);
        // Set empty messages sender.
        testedNode->onWrite([](const auto& /*msg*/){/*do nothing with the sent message*/ });
        auto begin = std::chrono::high_resolution_clock::now();
        {
            for (auto sink : sinks)
            {
                registry.addSink(sink);
                auto sinkName = sink->olinkObjectName();
                testedNode->linkRemote(sinkName);
                testedNode->handleMessage(initMessages.at(sinkName));
            }
            for (auto sink : sinks)
            {
                for ( auto i = 0u; i < messages_number; i++)
                {
                   sink->setPropInt(i);
                }
                testedNode->unlinkRemote(sink->olinkObjectName());
            }

        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        time.push_back(elapsed.count());
    }

    std::cout<<"average time measured "<< std::accumulate(time.begin(), time.end(), 0.0) / time.size()<< std::endl;
    auto max = std::max_element(time.begin(), time.end());
    auto min = std::min_element(time.begin(), time.end());
    std::cout<<"lowest time measured "<< *min << std::endl;
    std::cout<<"highest time measured "<< *max << std::endl;
    
    return 0;
}