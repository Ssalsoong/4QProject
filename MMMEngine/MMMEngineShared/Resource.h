#pragma once
#include "MUID.h"
#include "rttr/type"
#include "rttr/registration_friend.h"

namespace MMMEngine
{
	class Resource
	{
	private:
		//RTTR_ENABLE()				<- 이 친구도 엔트리 분리후 pak시스템 쓸 때 쓸 것 (RTTR은 결합도가 낮아야 함, 지금 방식에서는 이걸 쓰면 RTTR 결합도가 올라감)
		//RTTR_REGISTRATION_FRIEND
		friend class ResourceManager;

		//Utility::MUID m_muid; <- 쓸거면 엔트리 분리후에 pak시스템과 함께 써야함
		//inline void SetMUID(Utility::MUID muid) { m_muid = muid; }

		std::wstring m_filePath;
		inline void SetFilePath(const std::wstring& filePath) { m_filePath = filePath; }
	protected:
		//virtual bool LoadFromBytes(const void* data, size_t size) = 0;
		virtual bool LoadFromFilePath(const std::wstring& filePath) = 0;
	public:
		virtual ~Resource() = default;

		inline const std::wstring& GetFilePath() const { return m_filePath; }
		virtual const char* GetResourceTypeName() const = 0;

		//inline const Utility::MUID& GetMUID() const { return m_muid; }
	};
}

#define REGISTER_RESOURCE_TYPE(Type) \
    static const char* GetStaticTypeName() { return #Type; } \
    virtual const char* GetResourceTypeName() const override { return #Type; }