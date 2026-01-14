#pragma once
#include <memory>

template <typename T>
class SingleTon {
protected:
	SingleTon() = default;

public:
	SingleTon(const SingleTon& _other) = delete;
	SingleTon& operator=(const SingleTon& _other) = delete;

	static T* GetInstance() {
		static T instance;
		return &instance;
	}
};