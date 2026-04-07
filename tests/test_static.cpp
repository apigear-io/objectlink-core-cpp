// Standalone test compiled with OLINK_STATIC to verify the library
// compiles and links correctly without dllexport/dllimport decorations,
// as used in monolithic/static-only builds (e.g. Unreal Engine Shipping).

#include "olink/core/olink_common.h"
#include "olink/core/types.h"
#include "olink/core/protocol.h"
#include "olink/clientnode.h"
#include "olink/clientregistry.h"
#include "olink/remotenode.h"
#include "olink/remoteregistry.h"
#include "olink/consolelogger.h"

#include "nlohmann/json.hpp"

#include <cassert>
#include <cstdio>
#include <string>

using json = nlohmann::json;
using namespace ApiGear::ObjectLink;

static void test_name_utilities()
{
    std::string memberId = "demo.Calc/count";
    assert(Name::getObjectId(memberId) == "demo.Calc");
    assert(Name::getMemberName(memberId) == "count");
    assert(Name::isMemberId(memberId));
    assert(!Name::isMemberId("demo.Calc"));
    assert(Name::createMemberId("demo.Calc", "count") == memberId);
}

static void test_registries()
{
    ClientRegistry clientRegistry;
    clientRegistry.onLog(ConsoleLogger::logFunc());

    RemoteRegistry remoteRegistry;
    remoteRegistry.onLog(ConsoleLogger::logFunc());

    // Verify basic registry operations link correctly
    auto sink = clientRegistry.getSink("demo.Calc");
    assert(!sink.lock());

    auto source = remoteRegistry.getSource("demo.Calc");
    assert(!source.lock());
}

static void test_protocol()
{
    json linkMsg = Protocol::linkMessage("demo.Calc");
    assert(!linkMsg.empty());

    json initMsg = Protocol::initMessage("demo.Calc", json::object({{"count", 0}}));
    assert(!initMsg.empty());
}

int main()
{
    std::printf("test_name_utilities (OLINK_STATIC)... ");
    test_name_utilities();
    std::printf("OK\n");

    std::printf("test_registries (OLINK_STATIC)... ");
    test_registries();
    std::printf("OK\n");

    std::printf("test_protocol (OLINK_STATIC)... ");
    test_protocol();
    std::printf("OK\n");

    std::printf("\nAll static-build tests passed.\n");
    return 0;
}
