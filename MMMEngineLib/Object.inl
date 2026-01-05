#pragma once
#include "ObjectManager.h"

namespace MMMEngine
{
    template<typename T, typename... Args>
    ObjectPtr<T> Object::CreatePtr(Args&&... args)
    {
        return ObjectManager::Get()
            .CreatePtr<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    bool ObjectPtr<T>::IsValid() const
    {
        return ObjectManager::Get().IsValidPtr(m_ptrID, m_ptrGeneration, m_raw);
    }

    template<typename T>
    bool ObjectPtr<T>::IsSameObject(const ObjectPtrBase& other) const
    {
        if (m_ptrID != other.GetPtrID() ||
            m_ptrGeneration != other.GetPtrGeneration())
            return false;

        return IsValid() &&
            ObjectManager::Get().IsValidPtr(
                other.GetPtrID(),
                other.GetPtrGeneration(),
                other.GetBase()
            );
    }


    template<typename T>
    ObjectPtr<T> Object::FindObjectByType()
    {
        return ObjectManager::Get().FindObjectByType<T>();
    }

    template<typename T>
    std::vector<ObjectPtr<T>> Object::FindObjectsByType()
    {
        return ObjectManager::Get().FindObjectsByType<T>();
    }

    template<typename T>
    ObjectPtr<T> Object::SelfPtr(T* self)
    {
        assert(self == this && "Self의 인자가 자신이 아닙니다!");
        return ObjectManager::Get().GetPtrFast<T>(this, m_ptrID, m_ptrGen);
    }
}