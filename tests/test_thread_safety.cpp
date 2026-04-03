#include <catch2/catch.hpp>

#include "olink/core/uniqueidobjectstorage.h"
#include "olink/core/basenode.h"
#include "olink/core/types.h"

#include <memory>
#include <string>
#include <thread>
#include <future>
#include <vector>
#include <atomic>
#include <set>

using namespace ApiGear::ObjectLink;

// =====================================================================
// UniqueIdObjectStorage thread safety tests
// =====================================================================

class StorageTestObject {};

TEST_CASE("UniqueIdObjectStorage concurrent add/remove/get", "[ThreadSafety][UniqueIdObjectStorage]")
{
    UniqueIdObjectStorage<StorageTestObject> storage(100);
    constexpr std::size_t numThreads = 8;
    constexpr std::size_t opsPerThread = 50;

    // Create shared objects to add
    std::vector<std::shared_ptr<StorageTestObject>> objects;
    for (std::size_t i = 0; i < numThreads * opsPerThread; ++i) {
        objects.push_back(std::make_shared<StorageTestObject>());
    }

    SECTION("concurrent adds produce unique, valid ids")
    {
        std::vector<std::future<std::vector<unsigned long>>> futures;
        for (std::size_t t = 0; t < numThreads; ++t) {
            futures.push_back(std::async(std::launch::async, [&, t]() {
                std::vector<unsigned long> ids;
                for (std::size_t i = 0; i < opsPerThread; ++i) {
                    auto id = storage.add(objects[t * opsPerThread + i]);
                    ids.push_back(id);
                }
                return ids;
            }));
        }

        std::set<unsigned long> allIds;
        for (auto& f : futures) {
            auto ids = f.get();
            for (auto id : ids) {
                if (id != storage.getInvalidId()) {
                    REQUIRE(allIds.find(id) == allIds.end());
                    allIds.insert(id);
                }
            }
        }
        // All objects should have been added (100 slots, 400 objects attempted)
        // At least the first 100 should succeed
        REQUIRE(allIds.size() == 100);
    }

    SECTION("concurrent add and remove")
    {
        // Pre-add some objects
        std::vector<unsigned long> preAddedIds;
        for (std::size_t i = 0; i < 20; ++i) {
            preAddedIds.push_back(storage.add(objects[i]));
        }

        // Concurrently add new objects and remove existing ones
        auto adderFuture = std::async(std::launch::async, [&]() {
            for (std::size_t i = 20; i < 60; ++i) {
                storage.add(objects[i]);
            }
        });

        auto removerFuture = std::async(std::launch::async, [&]() {
            for (auto id : preAddedIds) {
                storage.remove(id);
            }
        });

        auto getterFuture = std::async(std::launch::async, [&]() {
            for (auto id : preAddedIds) {
                // get() should not crash, result may be expired or valid
                storage.get(id);
            }
        });

        adderFuture.get();
        removerFuture.get();
        getterFuture.get();

        // Removed objects should be gone
        for (auto id : preAddedIds) {
            REQUIRE(storage.get(id).expired());
        }
    }
}

// =====================================================================
// BaseNode thread safety tests
// =====================================================================

TEST_CASE("BaseNode concurrent emitWrite and onWrite", "[ThreadSafety][BaseNode]")
{
    // BaseNode is abstract via IProtocolListener, but we can use it directly
    // since the handler methods have default implementations
    struct TestNode : public BaseNode {};

    auto node = std::make_shared<TestNode>();
    std::atomic<int> writeCount{0};

    SECTION("concurrent emitWrite does not crash")
    {
        node->onWrite([&writeCount](const std::string&) {
            ++writeCount;
        });

        std::vector<std::future<void>> futures;
        for (int t = 0; t < 4; ++t) {
            futures.push_back(std::async(std::launch::async, [&]() {
                for (int i = 0; i < 50; ++i) {
                    nlohmann::json msg = {10, "obj1"};
                    node->emitWrite(msg);
                }
            }));
        }
        for (auto& f : futures) {
            f.get();
        }
        REQUIRE(writeCount == 200);
    }

    SECTION("changing write function while emitting does not crash")
    {
        std::atomic<int> funcACount{0};
        std::atomic<int> funcBCount{0};

        node->onWrite([&funcACount](const std::string&) {
            ++funcACount;
        });

        auto writerFuture = std::async(std::launch::async, [&]() {
            for (int i = 0; i < 100; ++i) {
                nlohmann::json msg = {10, "obj1"};
                node->emitWrite(msg);
            }
        });

        auto swapFuture = std::async(std::launch::async, [&]() {
            for (int i = 0; i < 50; ++i) {
                node->onWrite([&funcBCount](const std::string&) {
                    ++funcBCount;
                });
                node->onWrite([&funcACount](const std::string&) {
                    ++funcACount;
                });
            }
        });

        writerFuture.get();
        swapFuture.get();
        // Both counts together should equal 100
        REQUIRE(funcACount + funcBCount == 100);
    }

    SECTION("concurrent handleMessage and setMessageFormat does not crash")
    {
        node->onWrite([](const std::string&) {});

        auto handleFuture = std::async(std::launch::async, [&]() {
            for (int i = 0; i < 50; ++i) {
                // An invalid message, but it should not crash
                node->handleMessage("[]");
            }
        });

        auto formatFuture = std::async(std::launch::async, [&]() {
            for (int i = 0; i < 50; ++i) {
                node->setMessageFormat(MessageFormat::JSON);
            }
        });

        handleFuture.get();
        formatFuture.get();
        // No crash = success
    }
}

// =====================================================================
// LoggerBase thread safety tests
// =====================================================================

TEST_CASE("LoggerBase concurrent logging", "[ThreadSafety][LoggerBase]")
{
    struct TestLogger : public LoggerBase {};

    TestLogger logger;
    std::atomic<int> logCount{0};

    SECTION("concurrent emitLog does not crash")
    {
        logger.onLog([&logCount](LogLevel, const std::string&) {
            ++logCount;
        });
        logger.setLogLevel(LogLevel::Info);

        std::vector<std::future<void>> futures;
        for (int t = 0; t < 4; ++t) {
            futures.push_back(std::async(std::launch::async, [&]() {
                for (int i = 0; i < 50; ++i) {
                    logger.emitLog(LogLevel::Info, std::string("test msg"));
                }
            }));
        }
        for (auto& f : futures) {
            f.get();
        }
        REQUIRE(logCount == 200);
    }

    SECTION("changing log function while emitting does not crash")
    {
        std::atomic<int> funcACount{0};
        std::atomic<int> funcBCount{0};

        logger.onLog([&funcACount](LogLevel, const std::string&) {
            ++funcACount;
        });
        logger.setLogLevel(LogLevel::Info);

        auto logFuture = std::async(std::launch::async, [&]() {
            for (int i = 0; i < 100; ++i) {
                logger.emitLog(LogLevel::Info, std::string("msg"));
            }
        });

        auto swapFuture = std::async(std::launch::async, [&]() {
            for (int i = 0; i < 50; ++i) {
                logger.onLog([&funcBCount](LogLevel, const std::string&) {
                    ++funcBCount;
                });
                logger.onLog([&funcACount](LogLevel, const std::string&) {
                    ++funcACount;
                });
            }
        });

        logFuture.get();
        swapFuture.get();
        REQUIRE(funcACount + funcBCount == 100);
    }

    SECTION("changing log level while emitting does not crash")
    {
        logger.onLog([&logCount](LogLevel, const std::string&) {
            ++logCount;
        });
        logger.setLogLevel(LogLevel::Info);

        auto logFuture = std::async(std::launch::async, [&]() {
            for (int i = 0; i < 100; ++i) {
                logger.emitLog(LogLevel::Info, std::string("msg"));
            }
        });

        auto levelFuture = std::async(std::launch::async, [&]() {
            for (int i = 0; i < 50; ++i) {
                logger.setLogLevel(LogLevel::Error);
                logger.setLogLevel(LogLevel::Info);
            }
        });

        logFuture.get();
        levelFuture.get();
        // Some logs may be skipped due to level changes, but no crash
    }
}
