//Yes, pragma once is added here
// Also this file is not added to cmake.
// See the concrete template definition files that include this .cpp files, they are added to cmake
#pragma once

#include "uniqueidobjectstorage.h"
#include <algorithm>

namespace {
const unsigned long invalidId = 0xFFFFFFFFFFFFFFFF;
}

template<typename ObjectType>
UniqueIdObjectStorage<ObjectType>::UniqueIdObjectStorage(unsigned long maxCount)
: m_counter(0),
m_maxCount(maxCount)
{
    auto getUniqueAfterOverflow = [this]() -> unsigned long {
        auto currentId = m_counter;
        std::unique_lock<std::mutex> lock(m_counterMutex);
        while (m_objects.find(currentId) != m_objects.end()){
            currentId+=1;
            if (m_objects.size() == m_maxCount) return invalidId;
            if (currentId == m_maxCount){
                currentId = 0;
            }
        }
        m_counter = currentId + 1;
        if (m_counter == m_maxCount){
            m_counter = 0;
        }
        return currentId;
    };

    getUniqueId = [this, getUniqueAfterOverflow]() -> unsigned long {
        std::unique_lock<std::mutex> lock(m_counterMutex);
        unsigned long current_id = m_counter;
        m_counter+=1;
        if (m_counter == m_maxCount){
            m_counter = 0;
            getUniqueId = getUniqueAfterOverflow;
        }
        return current_id;
    };
}

template<typename ObjectType>
unsigned long UniqueIdObjectStorage<ObjectType>::add(std::weak_ptr<ObjectType> object)
{
    auto lockedObject = object.lock();
    if(!lockedObject) return invalidId;
    if (m_objects.size() == m_maxCount) return invalidId;

    std::unique_lock<std::mutex> lock(m_objectsMutex);

    auto alreadyAddedId = std::find_if(m_objects.begin(), m_objects.end(), 
            [lockedObject](auto current)
            {
                return !current.second.expired() &&
                    current.second.lock () == lockedObject;
            });
    if (alreadyAddedId != m_objects.end()) return alreadyAddedId->first;

    auto id = getUniqueId();
    m_objects[id] = object;
    return id;
}

template<typename ObjectType>
void UniqueIdObjectStorage<ObjectType>::remove(unsigned long id)
{
    if (m_objects.find(id) != m_objects.end())
    {
        std::unique_lock<std::mutex> lock(m_objectsMutex);
        m_objects.erase(id);
    }
}

template<typename ObjectType>
std::weak_ptr<ObjectType> UniqueIdObjectStorage<ObjectType>::get(unsigned long id)
{
    if (m_objects.find(id) != m_objects.end())
    {
        return m_objects[id];
    }
    return std::weak_ptr<ObjectType>();
}

template<typename ObjectType>
unsigned long UniqueIdObjectStorage<ObjectType>::getInvalidId() {
    return invalidId;
}