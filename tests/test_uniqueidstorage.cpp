#include <catch2/catch.hpp>

#include "olink/core/uniqueidobjectstorage.h"
#include <memory>

#include <string>
#include <future>


class MyTestObject
{
};
template class UniqueIdObjectStorage<MyTestObject>;

TEST_CASE("storage")
{
    auto obj1 = std::make_shared<MyTestObject>();
    auto obj2 = std::make_shared<MyTestObject>();
    auto obj3 = std::make_shared<MyTestObject>();
    auto obj4 = std::make_shared<MyTestObject>();
    auto obj5 = std::make_shared<MyTestObject>();
    auto obj6 = std::make_shared<MyTestObject>();
    auto obj7 = std::make_shared<MyTestObject>();

    unsigned long maxStorageCount = 5;

    UniqueIdObjectStorage<MyTestObject> storage(maxStorageCount);

    SECTION("each added object gets unique id")
    {
        auto id1 = storage.add(obj1);
        auto id2 = storage.add(obj2);
        auto id3 = storage.add(obj3);
        REQUIRE(id1 != id2);
        REQUIRE(id2 != id3);
        REQUIRE(id3 != id1);
        REQUIRE(storage.get(id1).lock() == obj1);
        REQUIRE(storage.get(id2).lock() == obj2);
        REQUIRE(storage.get(id3).lock() == obj3);
    }

    SECTION("one object is added only once, next adding return same id")
    {
        auto id1 = storage.add(obj1);
        REQUIRE(storage.add(obj1) == id1);
        REQUIRE(storage.get(id1).lock() == obj1);
    }

    SECTION("same object added after removal may get new id")
    {
        auto id1 = storage.add(obj1);
        storage.remove(id1);
        REQUIRE(storage.get(id1).expired() == true);
        auto id2 = storage.add(obj1);
        REQUIRE(id1 != id2);
        REQUIRE(storage.get(id2).lock() == obj1);
    }

    SECTION("adding object fails - storage full")
    {
        storage.add(obj1);
        storage.add(obj2);
        storage.add(obj3);
        storage.add(obj4);
        auto id5 = storage.add(obj5);
        auto id6 = storage.add(obj6);
        auto id7 = storage.add(obj7);
        REQUIRE(storage.get(id5).lock() == obj5);
        REQUIRE(storage.get(id6).expired() == true);
        REQUIRE(storage.get(id7).expired() == true);
        REQUIRE(id6 == storage.getInvalidId());
        REQUIRE(id7 == storage.getInvalidId());
    }

    SECTION("successful adding object after overflow")
    {
        auto id1 = storage.add(obj1);
        auto id2 = storage.add(obj2);
        storage.add(obj3);
        auto id4 = storage.add(obj4);
        storage.add(obj5);
        storage.remove(id2);
        storage.remove(id4);
        auto id6 = storage.add(obj6);
        auto id7 = storage.add(obj7);
        REQUIRE(storage.get(id6).lock() == obj6);
        REQUIRE(storage.get(id7).lock() == obj7);
        storage.remove(id1);
        id4 = storage.add(obj4);
        REQUIRE(storage.get(id4).lock() == obj4);
    }

    SECTION("unsuccessful adding object after overflow - again overflow")
    {
        storage.add(obj1);
        auto id2 = storage.add(obj2);
        storage.add(obj3);
        auto id4 = storage.add(obj4);
        storage.add(obj5);
        storage.remove(id2);
        storage.remove(id4);
        auto id6 = storage.add(obj6);
        auto id7 = storage.add(obj7);
        REQUIRE(storage.get(id6).lock() == obj6);
        REQUIRE(storage.get(id7).lock() == obj7);
        id4 = storage.add(obj4);
        REQUIRE(id4 == storage.getInvalidId());
        REQUIRE(storage.get(id4).expired() == true);
    }

    SECTION("purgeExpired removes expired weak_ptrs")
    {
        auto id1 = storage.add(obj1);
        auto id2 = storage.add(obj2);
        auto id3 = storage.add(obj3);
        REQUIRE(id1 != storage.getInvalidId());
        REQUIRE(id2 != storage.getInvalidId());
        REQUIRE(id3 != storage.getInvalidId());

        // Storage has 3 of 5 slots used. Let obj2 expire.
        obj2.reset();
        REQUIRE(storage.get(id2).expired());

        // purgeExpired should remove the expired entry.
        storage.purgeExpired();

        // The expired entry is gone, but the valid ones remain.
        REQUIRE(storage.get(id1).lock() == obj1);
        REQUIRE(storage.get(id2).expired());
        REQUIRE(storage.get(id3).lock() == obj3);
    }

    SECTION("purgeExpired allows adding to previously full storage")
    {
        unsigned long smallMax = 3;
        UniqueIdObjectStorage<MyTestObject> smallStorage(smallMax);

        auto a = std::make_shared<MyTestObject>();
        auto b = std::make_shared<MyTestObject>();
        auto c = std::make_shared<MyTestObject>();
        auto d = std::make_shared<MyTestObject>();

        auto idA = smallStorage.add(a);
        auto idB = smallStorage.add(b);
        auto idC = smallStorage.add(c);
        REQUIRE(idA != smallStorage.getInvalidId());
        REQUIRE(idB != smallStorage.getInvalidId());
        REQUIRE(idC != smallStorage.getInvalidId());

        // Storage is full - adding should fail.
        auto idFail = smallStorage.add(d);
        REQUIRE(idFail == smallStorage.getInvalidId());

        // Let one expire.
        b.reset();
        REQUIRE(smallStorage.get(idB).expired());

        // Explicit purge, then add.
        smallStorage.purgeExpired();
        auto idD = smallStorage.add(d);
        REQUIRE(idD != smallStorage.getInvalidId());
        REQUIRE(smallStorage.get(idD).lock() == d);
    }

    SECTION("add lazily purges expired entries when storage is full")
    {
        unsigned long smallMax = 3;
        UniqueIdObjectStorage<MyTestObject> smallStorage(smallMax);

        auto a = std::make_shared<MyTestObject>();
        auto b = std::make_shared<MyTestObject>();
        auto c = std::make_shared<MyTestObject>();
        auto d = std::make_shared<MyTestObject>();

        smallStorage.add(a);
        smallStorage.add(b);
        smallStorage.add(c);

        // Storage is full. Let two expire.
        a.reset();
        b.reset();

        // add() should lazily purge and succeed without explicit purgeExpired().
        auto idD = smallStorage.add(d);
        REQUIRE(idD != smallStorage.getInvalidId());
        REQUIRE(smallStorage.get(idD).lock() == d);
    }

    SECTION("Multithreaded adding and remove")
    {
        auto id1 = storage.add(obj1);
        auto future1 = std::async(std::launch::async, [obj2, &storage](){return storage.add(obj2); });
        auto future2 = std::async(std::launch::async, [obj3, &storage](){return storage.add(obj3); });
        auto future3 = std::async(std::launch::async, [id1, &storage](){storage.remove(id1); });
        auto future4 = std::async(std::launch::async, [obj4, &storage](){return storage.add(obj4); });
        auto future5 = std::async(std::launch::async, [obj5, &storage](){return storage.add(obj5); });
        REQUIRE(storage.get(future1.get()).lock() == obj2);
        REQUIRE(storage.get(future2.get()).lock() == obj3);
        REQUIRE(storage.get(future4.get()).lock() == obj4);
        REQUIRE(storage.get(future5.get()).lock() == obj5);
        future3.get();
        REQUIRE(storage.get(id1).expired() == true);
    }
}
