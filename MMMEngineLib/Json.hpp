#include "json/json.hpp"
using json = nlohmann::json;

namespace MMMEngine
{
	class Json {
	public:
		template <typename T>
		static std::string serialize(const T& object)
		{
			json j = object; // T::to_json(j, object) 호출
			return j.dump(4); // 가독성을 위해 들여쓰기 4칸 적용
		}

		template <typename T>
		static T deserialize(const std::string& json_string)
		{
			try {
				json j = json::parse(json_string);
				return j.get<T>(); // T::from_json(j, object) 호출
			}
			catch (const json::parse_error& e) {
				// 파싱 오류 처리 (JSON 문법 오류 등)
				throw std::runtime_error("JSON Parse Error: " + std::string(e.what()));
			}
			catch (const std::exception& e) {
				// 일반 역직렬화 오류 처리
				throw std::runtime_error("Deserialization Error: " + std::string(e.what()));
			}
		}

		template <typename T>
		static T load_from_file(const std::string& filename)
		{
			std::ifstream file(filename);
			if (!file.is_open()) {
				throw std::runtime_error("Failed to open file for loading: " + filename);
			}

			try {
				json j;
				file >> j; // Nlohmann JSON의 파일 스트림 연산자 사용
				return j.get<T>();
			}
			catch (const json::exception& e) {
				throw std::runtime_error("JSON load/parse error in file " + filename + ": " + e.what());
			}
		}

		template <typename T>
		static bool save_to_file(const std::string& filename, const T& object)
		{
			std::ofstream file(filename);
			if (!file.is_open()) {
				return false; // 파일 열기 실패
			}

			try {
				json j = object; // T::to_json 호출
				file << j.dump(4); // 들여쓰기 4칸 적용하여 저장
				return true;
			}
			catch (const std::exception& e) {
				// 직렬화 과정 중 오류 발생
				// 실제 환경에서는 로그 기록 등의 추가 작업 필요
				return false;
			}
		}
	};
}
