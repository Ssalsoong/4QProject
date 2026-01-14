#pragma once
#include "framework.h"
#include "Component.h"
#include <memory>

class Transform : public Component
{
private:
	DirectX::SimpleMath::Matrix m_World = DirectX::SimpleMath::Matrix::Identity;	// 월드좌표계 변환행렬
	std::weak_ptr<Transform> parent;	// Parent
public:
	void Start() override;
	void Update() override;

	void SetParent(std::weak_ptr<Transform>& _parent);
	std::weak_ptr<Transform> GetParent() const;
	const DirectX::SimpleMath::Matrix& GetMatrix() const;

	DirectX::SimpleMath::Vector3 Position = DirectX::SimpleMath::Vector3::Zero;		// x, y, z Position
	DirectX::SimpleMath::Vector3 Rotation = DirectX::SimpleMath::Vector3::Zero;		// pitch, yaw, roll
	DirectX::SimpleMath::Vector3 Scale = { 1.0f, 1.0f, 1.0f };						// x, y, z Scale
};

