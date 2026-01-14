#pragma once
#include "Export.h"

namespace MMMEngine::Utility {
    template<typename T>
    class ExportSingleton {
    protected:
        ExportSingleton() = default;
        virtual ~ExportSingleton() = default;

    public:
        ExportSingleton(const ExportSingleton&) = delete;
        ExportSingleton& operator=(const ExportSingleton&) = delete;

        // DLL에서 안전하게 사용
        static T& Get() {
            return GetInstanceImpl();
        }

    private:
        static T& GetInstanceImpl();
    };
}

// 각 싱글톤 클래스마다 .cpp에 추가
#define DEFINE_SINGLETON(ClassName) \
    template<> \
    MMMENGINE_API ClassName& MMMEngine::Utility::ExportSingleton<ClassName>::GetInstanceImpl() { \
        static ClassName instance; \
        return instance; \
    }