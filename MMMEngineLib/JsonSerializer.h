#pragma once
#define RTTR_DLL
#include "rttr/registration"
#include "rttr/detail/policies/ctor_policies.h"

using namespace rttr;

#include "json/json.hpp"

using namespace nlohmann::json_abi_v3_12_0;

#include <type_traits>
#include "Object.h"

namespace MMMEngine
{
	class JsonSerializer
	{
	private:
		std::optional<json> SerializeVariant(const variant& var);
		bool DeserializeVariant(const json& j, variant& var);
	public:
		json Serialize(const Object& obj);

		bool Deserialize(const json& j, Object& obj);
	};

	inline json JsonSerializer::Serialize(const Object& obj)
	{

		nlohmann::json objJson;

		type t = type::get(obj);
		objJson["type"] = t.get_name().to_string();
		objJson["properties"] = nlohmann::json::object();

		for (auto& prop : t.get_properties())
		{
			std::string propName = prop.get_name().to_string();
			variant value = prop.get_value(obj);

			if (auto jsonValue = SerializeVariant(value))
			{
				objJson["properties"][propName] = *jsonValue;
			}
		}

		return objJson;
	}

	inline bool JsonSerializer::Deserialize(const json& j, Object& obj)
	{
		if (!j.contains("properties") || !j["properties"].is_object())
			return false;

		type t = type::get(obj);

		for (auto& prop : t.get_properties())
		{
			std::string propName = prop.get_name().to_string();

			if (!j["properties"].contains(propName))
				continue;

			variant propValue = prop.get_value(obj);
			if (DeserializeVariant(j["properties"][propName], propValue))
			{
				prop.set_value(obj, propValue);
			}
		}

		return true;
	}
}
