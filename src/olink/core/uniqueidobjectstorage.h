#pragma once

#include <memory>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <functional>

/*
* Helper class that stores weak_ptr of ObjectType and assigns it unique Id.
* The class can be used in multithreaded app.
*/
template<typename ObjectType>
class UniqueIdObjectStorage {
public:
    /**ctor
    * @param maxCount Maximum number of objects hold by this storage. By default it is highest unsigned long value.
    */
    UniqueIdObjectStorage(unsigned long maxCount = 0xFFFFFFFFFFFFFFFF);
    
    /*
    * Adds an object to a storage and assigns an id for it.
    * One object is stored only once, if it already exists in the storage then no new id is generated and old one is used.
    * @param object. An object that will be added to a storage.
    * @return A unique id for added object. If adding failed the invalid id is returned.
    */
    unsigned long add(std::weak_ptr<ObjectType> object);
    
    /*
    * Removes item from storage.
    * @param id The id of object that is to be removed from storage.
    */
    void remove(unsigned long id);
    
    /*
    * Gives access to stored object given by id.
    * @param id The id of object that should be obtained.
    * @return The object found for given id or expired pointer if object is not in the storage.
    */
    std::weak_ptr<ObjectType> get(unsigned long id);

    /*
    * @return An id that is considered as invalid in this storage. It is the maximum value of unsigned long.
    */
    unsigned long getInvalidId();
private:
    /*
    * A function that generates id.
    * @returns the unique id to be used among m_objects.
    */
    std::function<unsigned long(void)> getUniqueId;
    /* Maximum number of objects hold by this storage. */
    const unsigned int m_maxCount;
    
    /* Object stored with their unique id. */
    std::unordered_map<unsigned long, std::weak_ptr<ObjectType>> m_objects;
    /* A mutex to guard operations on stored objects.*/
    std::mutex m_objectsMutex;
    
    /* A counter to help tracking unique id*/
    unsigned long m_counter;
    /* A mutex to guard operations on counter.*/
    std::mutex m_counterMutex;
};
