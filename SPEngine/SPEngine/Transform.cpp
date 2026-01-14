#include "Transform.h"
#include <memory>

using namespace DirectX::SimpleMath;

void Transform::Start()
{

}

void Transform::Update()
{
	m_World =
		Matrix::CreateScale(Scale) *
		Matrix::CreateFromYawPitchRoll(Rotation.y, Rotation.x, Rotation.z) *
		Matrix::CreateTranslation(Position);
}

void Transform::SetParent(std::weak_ptr<Transform>& _parent)
{
	parent = _parent;
}

std::weak_ptr<Transform> Transform::GetParent() const
{
	return parent;
}

const DirectX::SimpleMath::Matrix& Transform::GetMatrix() const
{
	return m_World;
}
