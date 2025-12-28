/*
    Based on "The Impossibly Fast C++ Delegates"
    Copyright (C) 2017 Sergey A Kryukov, 2005 Sergey Ryazanov
    MIT License - See THIRD_PARTY_NOTICES.txt
*/

#pragma once
#include "fast_delegate/MultiCastDelegate.h"
#include <unordered_map>
#include <functional>
#include <stdexcept>

namespace MMMEngine
{
    // 함수 시그니처를 고유하게 식별하기 위한 키
    struct FunctionKey {
        void* objectPtr;  // 객체 포인터 (자유 함수는 nullptr)
        void* functionPtr; // 함수 포인터

        bool operator==(const FunctionKey& other) const {
            return objectPtr == other.objectPtr && functionPtr == other.functionPtr;
        }
    };

    // FunctionKey를 위한 해시 함수
    struct FunctionKeyHash {
        std::size_t operator()(const FunctionKey& key) const {
            return std::hash<void*>()(key.objectPtr) ^
                (std::hash<void*>()(key.functionPtr) << 1);
        }
    };

    // Action: void 반환형 델리게이트
    template<typename... Args>
    class Action {
    public:
        using FunctionPtr = void(*)(Args...);
        using DelegateType = SA::delegate<void(Args...)>;
        using MulticastType = SA::multicast_delegate<void(Args...)>;

        Action() = default;
        ~Action() = default;

        // 자유 함수 추가 (함수 포인터 직접 받기)
        void AddListener(FunctionPtr function) {
            FunctionKey key{ nullptr, reinterpret_cast<void*>(function) };

            if (m_functionMap.find(key) != m_functionMap.end())
                return; // 이미 등록됨

            auto delegate = DelegateType([function](Args... args) {
                function(args...);
                });
            m_functionMap[key] = delegate;
            m_multicast += delegate;
        }

        // 멤버 함수 추가 (std::bind 스타일)
        template<typename T>
        void AddListener(T* instance, void(T::* method)(Args...)) {
            void* methodPtr = *reinterpret_cast<void**>(&method);
            FunctionKey key{ instance, methodPtr };

            if (m_functionMap.find(key) != m_functionMap.end())
                return; // 이미 등록됨

            auto delegate = DelegateType([instance, method](Args... args) {
                (instance->*method)(args...);
                });
            m_functionMap[key] = delegate;
            m_multicast += delegate;
        }

        // Const 멤버 함수 추가
        template<typename T>
        void AddListener(const T* instance, void(T::* method)(Args...) const) {
            void* methodPtr = *reinterpret_cast<void**>(const_cast<void(T::**)(Args...) const>(&method));
            FunctionKey key{ const_cast<T*>(instance), methodPtr };

            if (m_functionMap.find(key) != m_functionMap.end())
                return;

            auto delegate = DelegateType([instance, method](Args... args) {
                (instance->*method)(args...);
                });
            m_functionMap[key] = delegate;
            m_multicast += delegate;
        }

        // 람다 추가 (제거 불가능, 하지만 += 연산자로 사용 가능)
        template<typename Lambda>
        void AddListener(const Lambda& lambda) {
            auto delegate = DelegateType(lambda);
            m_multicast += delegate;
        }

        // 람다용 += 연산자 (함수 포인터가 아닌 모든 callable)
        template<typename Lambda>
        Action& operator+=(const Lambda& lambda) {
            AddListener(lambda);
            return *this;
        }

        // 자유 함수 제거
        bool RemoveListener(FunctionPtr function) {
            FunctionKey key{ nullptr, reinterpret_cast<void*>(function) };

            auto it = m_functionMap.find(key);
            if (it == m_functionMap.end())
                return false;

            m_multicast -= it->second;
            m_functionMap.erase(it);
            return true;
        }

        // 멤버 함수 제거
        template<typename T>
        bool RemoveListener(T* instance, void(T::* method)(Args...)) {
            void* methodPtr = *reinterpret_cast<void**>(&method);
            FunctionKey key{ instance, methodPtr };

            auto it = m_functionMap.find(key);
            if (it == m_functionMap.end())
                return false;

            m_multicast -= it->second;
            m_functionMap.erase(it);
            return true;
        }

        // Const 멤버 함수 제거
        template<typename T>
        bool RemoveListener(const T* instance, void(T::* method)(Args...) const) {
            void* methodPtr = *reinterpret_cast<void**>(const_cast<void(T::**)(Args...) const>(&method));
            FunctionKey key{ const_cast<T*>(instance), methodPtr };

            auto it = m_functionMap.find(key);
            if (it == m_functionMap.end())
                return false;

            m_multicast -= it->second;
            m_functionMap.erase(it);
            return true;
        }

        // -= 연산자 (자유 함수만 가능)
        Action& operator-=(FunctionPtr function) {
            RemoveListener(function);
            return *this;
        }

        void Clear() {
            m_multicast.clear();
            m_functionMap.clear();
        }

        size_t Count() const {
            return m_multicast.size();
        }

        void Invoke(Args... args) const {
            m_multicast(args...);
        }

        void operator()(Args... args) const {
            Invoke(args...);
        }

    private:
        MulticastType m_multicast;
        std::unordered_map<FunctionKey, DelegateType, FunctionKeyHash> m_functionMap;
    };

    // Func: 반환값이 있는 델리게이트
    template<typename R, typename... Args>
    class Func {
    public:
        using FunctionPtr = R(*)(Args...);
        using DelegateType = SA::delegate<R(Args...)>;
        using MulticastType = SA::multicast_delegate<R(Args...)>;

        Func() = default;
        ~Func() = default;

        // 자유 함수 추가
        void AddListener(FunctionPtr function) {
            FunctionKey key{ nullptr, reinterpret_cast<void*>(function) };

            if (m_functionMap.find(key) != m_functionMap.end())
                return;

            auto delegate = DelegateType([function](Args... args) -> R {
                return function(args...);
                });
            m_functionMap[key] = delegate;
            m_multicast += delegate;
        }

        // 멤버 함수 추가
        template<typename T>
        void AddListener(T* instance, R(T::* method)(Args...)) {
            void* methodPtr = *reinterpret_cast<void**>(&method);
            FunctionKey key{ instance, methodPtr };

            if (m_functionMap.find(key) != m_functionMap.end())
                return;

            auto delegate = DelegateType([instance, method](Args... args) -> R {
                return (instance->*method)(args...);
                });
            m_functionMap[key] = delegate;
            m_multicast += delegate;
        }

        // Const 멤버 함수 추가
        template<typename T>
        void AddListener(const T* instance, R(T::* method)(Args...) const) {
            void* methodPtr = *reinterpret_cast<void**>(const_cast<R(T::**)(Args...) const>(&method));
            FunctionKey key{ const_cast<T*>(instance), methodPtr };

            if (m_functionMap.find(key) != m_functionMap.end())
                return;

            auto delegate = DelegateType([instance, method](Args... args) -> R {
                return (instance->*method)(args...);
                });
            m_functionMap[key] = delegate;
            m_multicast += delegate;
        }

        // 람다 추가
        template<typename Lambda>
        void AddListener(const Lambda& lambda) {
            auto delegate = DelegateType(lambda);
            m_multicast += delegate;
        }

        // 람다용 += 연산자
        template<typename Lambda>
        Func& operator+=(const Lambda& lambda) {
            AddListener(lambda);
            return *this;
        }

        // 자유 함수 제거
        bool RemoveListener(FunctionPtr function) {
            FunctionKey key{ nullptr, reinterpret_cast<void*>(function) };

            auto it = m_functionMap.find(key);
            if (it == m_functionMap.end())
                return false;

            m_multicast -= it->second;
            m_functionMap.erase(it);
            return true;
        }

        // 멤버 함수 제거
        template<typename T>
        bool RemoveListener(T* instance, R(T::* method)(Args...)) {
            void* methodPtr = *reinterpret_cast<void**>(&method);
            FunctionKey key{ instance, methodPtr };

            auto it = m_functionMap.find(key);
            if (it == m_functionMap.end())
                return false;

            m_multicast -= it->second;
            m_functionMap.erase(it);
            return true;
        }

        // Const 멤버 함수 제거
        template<typename T>
        bool RemoveListener(const T* instance, R(T::* method)(Args...) const) {
            void* methodPtr = *reinterpret_cast<void**>(const_cast<R(T::**)(Args...) const>(&method));
            FunctionKey key{ const_cast<T*>(instance), methodPtr };

            auto it = m_functionMap.find(key);
            if (it == m_functionMap.end())
                return false;

            m_multicast -= it->second;
            m_functionMap.erase(it);
            return true;
        }

        // -= 연산자
        Func& operator-=(FunctionPtr function) {
            RemoveListener(function);
            return *this;
        }

        void Clear() {
            m_multicast.clear();
            m_functionMap.clear();
        }

        size_t Count() const {
            return m_multicast.size();
        }

        R Invoke(Args... args) const {
            return m_multicast(args...);
        }

        R operator()(Args... args) const {
            return Invoke(args...);
        }

        template<typename Handler>
        void InvokeWithHandler(Args... args, Handler handler) const {
            m_multicast(args..., handler);
        }

    private:
        MulticastType m_multicast;
        std::unordered_map<FunctionKey, DelegateType, FunctionKeyHash> m_functionMap;
    };

    // ----------------------------------------
    // Event: 소유자만 Invoke 가능한 델리게이트
    // 
    // 작성 요령 : 
    //  Event는 소유자 클래스의 멤버로 사용해야합니다.
    //  템플릿의 첫 인자에는 클래스의 타입을 명시합니다.
    //  초기화문의 인자에는 this를 넣어주세요.
    //  
    //  요약하자면 아래와 같이 작성해주세요
    // 
    //  Event<OwnerType, void(void)> OnUpdate{ this };
    // 
    // 
    //  소유자는 아래와 같은 방법으로 Invoke를 호출합니다.
    // 
    //  ( * 클래스 멤버 함수 내에서 )
    //  OnReceive.Invoke(this, arg1, arg2);
    //  OnReceive(this, arg1, arg2);
    // ----------------------------------------

    template<typename OwnerType, typename Signature>
    class Event;

    template<typename OwnerType, typename R, typename... Args>
    class Event<OwnerType, R(Args...)> {
    public:
        using FunctionPtr = R(*)(Args...);
        using DelegateType = SA::delegate<R(Args...)>;
        using MulticastType = SA::multicast_delegate<R(Args...)>;

        explicit Event(OwnerType* owner) : m_owner(owner) {
            if (owner == nullptr) {
                throw std::invalid_argument("Event owner cannot be nullptr");
            }
        }

        Event(std::nullptr_t) = delete;

        // 자유 함수 추가
        void AddListener(FunctionPtr function) {
            FunctionKey key{ nullptr, reinterpret_cast<void*>(function) };

            if (m_functionMap.find(key) != m_functionMap.end())
                return;

            auto delegate = DelegateType([function](Args... args) -> R {
                if constexpr (std::is_same_v<R, void>) {
                    function(args...);
                }
                else {
                    return function(args...);
                }
                });
            m_functionMap[key] = delegate;
            m_multicast += delegate;
        }

        // 멤버 함수 추가
        template<typename T>
        void AddListener(T* instance, R(T::* method)(Args...)) {
            void* methodPtr = *reinterpret_cast<void**>(&method);
            FunctionKey key{ instance, methodPtr };

            if (m_functionMap.find(key) != m_functionMap.end())
                return;

            auto delegate = DelegateType([instance, method](Args... args) -> R {
                if constexpr (std::is_same_v<R, void>) {
                    (instance->*method)(args...);
                }
                else {
                    return (instance->*method)(args...);
                }
                });
            m_functionMap[key] = delegate;
            m_multicast += delegate;
        }

        // Const 멤버 함수 추가
        template<typename T>
        void AddListener(const T* instance, R(T::* method)(Args...) const) {
            void* methodPtr = *reinterpret_cast<void**>(const_cast<R(T::**)(Args...) const>(&method));
            FunctionKey key{ const_cast<T*>(instance), methodPtr };

            if (m_functionMap.find(key) != m_functionMap.end())
                return;

            auto delegate = DelegateType([instance, method](Args... args) -> R {
                if constexpr (std::is_same_v<R, void>) {
                    (instance->*method)(args...);
                }
                else {
                    return (instance->*method)(args...);
                }
                });
            m_functionMap[key] = delegate;
            m_multicast += delegate;
        }

        // 람다 추가
        template<typename Lambda>
        void AddListener(const Lambda& lambda) {
            auto delegate = DelegateType(lambda);
            m_multicast += delegate;
        }

        // 람다용 += 연산자
        template<typename Lambda>
        Event& operator+=(const Lambda& lambda) {
            AddListener(lambda);
            return *this;
        }

        // 자유 함수 제거
        bool RemoveListener(FunctionPtr function) {
            FunctionKey key{ nullptr, reinterpret_cast<void*>(function) };

            auto it = m_functionMap.find(key);
            if (it == m_functionMap.end())
                return false;

            m_multicast -= it->second;
            m_functionMap.erase(it);
            return true;
        }

        // 멤버 함수 제거
        template<typename T>
        bool RemoveListener(T* instance, R(T::* method)(Args...)) {
            void* methodPtr = *reinterpret_cast<void**>(&method);
            FunctionKey key{ instance, methodPtr };

            auto it = m_functionMap.find(key);
            if (it == m_functionMap.end())
                return false;

            m_multicast -= it->second;
            m_functionMap.erase(it);
            return true;
        }

        // Const 멤버 함수 제거
        template<typename T>
        bool RemoveListener(const T* instance, R(T::* method)(Args...) const) {
            void* methodPtr = *reinterpret_cast<void**>(const_cast<R(T::**)(Args...) const>(&method));
            FunctionKey key{ const_cast<T*>(instance), methodPtr };

            auto it = m_functionMap.find(key);
            if (it == m_functionMap.end())
                return false;

            m_multicast -= it->second;
            m_functionMap.erase(it);
            return true;
        }

        // -= 연산자
        Event& operator-=(FunctionPtr function) {
            RemoveListener(function);
            return *this;
        }

        void Clear() {
            m_multicast.clear();
            m_functionMap.clear();
        }

        size_t Count() const {
            return m_multicast.size();
        }

    private:
        OwnerType* m_owner;
        MulticastType m_multicast;
        std::unordered_map<FunctionKey, DelegateType, FunctionKeyHash> m_functionMap;

        friend OwnerType;

        // 소유자만 호출 가능
        void Invoke(OwnerType* owner, Args... args) {
            if (owner != m_owner)
                throw std::runtime_error("Event invoked by non-owner");

            if constexpr (std::is_same_v<R, void>) {
                m_multicast(args...);
            }
            else {
                return m_multicast(args...);
            }
        }

        auto operator()(OwnerType* owner, Args... args) {
            return Invoke(owner, args...);
        }
    };
} // namespace MMMEngine