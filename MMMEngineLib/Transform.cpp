#include "Transform.h"

void MMMEngine::Transform::AddChild(ObjectPtr<Transform> child)
{
	if (child == nullptr || child == SelfPtr(this)) return; // 자기 자신이나 nullptr은 추가하지 않음
	// 이미 자식으로 등록되어 있는지 확인
	for (const auto& existingChild : m_childs)
	{
		if (existingChild == child) return; // 이미 자식으로 등록되어 있으면 추가하지 않음
	}
	m_childs.push_back(child);
	child->m_parent = SelfPtr(this); // 부모 설정
}

void MMMEngine::Transform::RemoveChild(ObjectPtr<Transform> child)
{
	if (child == nullptr) return; // nullptr은 제거하지 않음
	auto it = std::find(m_childs.begin(), m_childs.end(), child);
	if (it != m_childs.end())
	{
		m_childs.erase(it); // 자식 목록에서 제거
		child->m_parent = nullptr; // 부모 설정 해제
	}
}

void MMMEngine::Transform::MarkDirty()
{
	m_isMatrixDirty = true;
	for (auto child : m_childs)
	{
		child->MarkDirty(); // 자식들도 더러워졌다고 표시
	}
}

MMMEngine::Transform::Transform()
	: m_localPosition(0.0f, 0.0f, 0.0f)
	, m_localRotation(0.0f, 0.0f, 0.0f, 1.0f)
	, m_localScale(1.0f, 1.0f, 1.0f)
	, m_parent()
	, m_isMatrixDirty(true)
	, m_cachedMatrix(Matrix::Identity)
{

}

MMMEngine::Transform::~Transform()
{
	if (!m_childs.empty())
	{
		for (auto child : m_childs)
		{
			child->SetParent(nullptr);
		}
	}
	SetParent(nullptr);
}

Matrix& MMMEngine::Transform::GetLocalMatrix() const
{
	if (m_isMatrixDirty)
	{
		m_cachedMatrix =
			Matrix::CreateScale(m_localScale) *
			Matrix::CreateFromQuaternion(m_localRotation) *
			Matrix::CreateTranslation(m_localPosition);

		m_isMatrixDirty = false;
	}

	return m_cachedMatrix;
}

Matrix MMMEngine::Transform::GetWorldMatrix() const
{
	if (m_parent)
		return GetLocalMatrix() * m_parent->GetWorldMatrix();
	else
		return GetLocalMatrix();
}

const Vector3& MMMEngine::Transform::GetLocalPosition() const
{
	return m_localPosition;
}

const Quaternion& MMMEngine::Transform::GetLocalRotation() const
{
	return m_localRotation;
}

const Vector3 MMMEngine::Transform::GetLocalEulerRotation() const
{
	auto euler = m_localRotation.ToEuler();
	euler.x = DirectX::XMConvertToDegrees(euler.x);
	euler.y = DirectX::XMConvertToDegrees(euler.y);
	euler.z = DirectX::XMConvertToDegrees(euler.z);
	return euler;
}

const Vector3& MMMEngine::Transform::GetLocalScale() const
{
	return m_localScale;
}

const Vector3 MMMEngine::Transform::GetWorldPosition() const
{
	if (!m_parent) return m_localPosition;

	const Vector3 pPos = m_parent->GetWorldPosition();
	const Quaternion pRot = m_parent->GetWorldRotation();
	const Vector3 pScale = m_parent->GetWorldScale();

	Vector3 scaled = Vector3(m_localPosition.x * pScale.x,
		m_localPosition.y * pScale.y,
		m_localPosition.z * pScale.z);

	Vector3 rotated = Vector3::Transform(scaled, pRot);
	return pPos + rotated;
}

const Quaternion MMMEngine::Transform::GetWorldRotation() const
{
	if (m_parent)
	{
		return m_parent->GetWorldRotation() * m_localRotation;
	}
	return m_localRotation;
}

const Vector3 MMMEngine::Transform::GetWorldEulerRotation() const
{
	auto worldRot = GetWorldRotation();
	auto euler = worldRot.ToEuler();
	euler.x = DirectX::XMConvertToDegrees(euler.x);
	euler.y = DirectX::XMConvertToDegrees(euler.y);
	euler.z = DirectX::XMConvertToDegrees(euler.z);
	return euler;
}

const Vector3 MMMEngine::Transform::GetWorldScale() const
{
	if (m_parent)
	{
		Vector3 parentScale = m_parent->GetWorldScale();
		return Vector3{
			m_localScale.x * parentScale.x,
			m_localScale.y * parentScale.y,
			m_localScale.z * parentScale.z
		};
	}
	return m_localScale;
}

void MMMEngine::Transform::SetWorldPosition(Vector3 pos)
{
	if (!m_parent)
	{
		m_localPosition = pos;
	}
	else
	{
		const Vector3 pPos = m_parent->GetWorldPosition();
		const Quaternion pRot = m_parent->GetWorldRotation();
		const Vector3 pScale = m_parent->GetWorldScale();

		Vector3 v = pos - pPos;

		Quaternion invRot = pRot;
		invRot.Inverse(invRot); // 또는 pRot.Inverse(...)
		v = Vector3::Transform(v, invRot);

		const float eps = 1e-6f;
		m_localPosition = Vector3(
			(fabs(pScale.x) > eps) ? v.x / pScale.x : v.x,
			(fabs(pScale.y) > eps) ? v.y / pScale.y : v.y,
			(fabs(pScale.z) > eps) ? v.z / pScale.z : v.z
		);
	}

	MarkDirty();
	onMatrixUpdate.Invoke(this);
}

void MMMEngine::Transform::SetWorldRotation(Quaternion rot)
{
	if (m_parent)
	{
		Quaternion parentInvQuater = Quaternion::Identity;
		m_parent->GetWorldRotation().Inverse(parentInvQuater);

		m_localRotation = parentInvQuater * rot;
	}
	else
	{
		m_localRotation = rot;
	}

	MarkDirty();
	onMatrixUpdate.Invoke(this);
}

void MMMEngine::Transform::SetWorldEulerRotation(Vector3 rot)
{
	// 오일러 각(도 단위)을 라디안 단위로 변환
	auto radRotX = DirectX::XMConvertToRadians(rot.x);
	auto radRotY = DirectX::XMConvertToRadians(rot.y);
	auto radRotZ = DirectX::XMConvertToRadians(rot.z);

	// 라디안 오일러 각으로 쿼터니언 생성
	Quaternion worldRot = Quaternion::CreateFromYawPitchRoll(radRotY, radRotX, radRotZ);

	SetWorldRotation(worldRot);
}

void MMMEngine::Transform::SetWorldScale(Vector3 scale)
{
	if (m_parent)
	{
		Vector3 parentScale = m_parent->GetWorldScale();
		const float epsilon = 1e-6f;
		m_localScale = Vector3(
			(abs(parentScale.x) > epsilon) ? scale.x / parentScale.x : scale.x,
			(abs(parentScale.y) > epsilon) ? scale.y / parentScale.y : scale.y,
			(abs(parentScale.z) > epsilon) ? scale.z / parentScale.z : scale.z
		);
	}
	else
	{
		m_localScale = scale;
	}

	MarkDirty();
	onMatrixUpdate.Invoke(this);
}

void MMMEngine::Transform::SetLocalPosition(Vector3 pos)
{
	m_localPosition = pos;
	MarkDirty();
	onMatrixUpdate.Invoke(this);
}

void MMMEngine::Transform::SetLocalRotation(Quaternion rot)
{
	m_localRotation = rot;
	MarkDirty();
	onMatrixUpdate.Invoke(this);
}

void MMMEngine::Transform::SetParent(ObjectPtr<Transform> parent, bool worldPositionStays)
{
	// 이미 같은 부모면 return
	if (m_parent == parent) return;

	// 현재 월드 포지션 백업
	const auto worldScaleBefore = GetWorldScale();
	const auto worldRotationBefore = GetWorldRotation();
	const auto worldPositionBefore = GetWorldPosition();

	// 기존 부모에서 제거
	if (m_parent)
		m_parent->RemoveChild(SelfPtr(this));


	// 부모 교체
	m_parent = parent;

	if (m_parent)
		m_parent->AddChild(SelfPtr(this));

	if (worldPositionStays)
	{
		if (m_parent)
		{
			// 부모 기준 상 로컬 행렬
			auto pScale = m_parent->GetWorldScale();
			auto pRot = m_parent->GetWorldRotation();
			auto pPos = m_parent->GetWorldPosition();

			Quaternion invRot = pRot;
			invRot.Inverse(invRot);

			const float eps = 1e-6f;

			// scale
			m_localScale = Vector3(
				(fabs(pScale.x) > eps) ? worldScaleBefore.x / pScale.x : worldScaleBefore.x,
				(fabs(pScale.y) > eps) ? worldScaleBefore.y / pScale.y : worldScaleBefore.y,
				(fabs(pScale.z) > eps) ? worldScaleBefore.z / pScale.z : worldScaleBefore.z
			);


			m_localRotation = invRot * worldRotationBefore;
			m_localRotation.Normalize();

			Vector3 v = worldPositionBefore - pPos;
			v = Vector3::Transform(v, invRot);

			m_localPosition = Vector3(
				(fabs(pScale.x) > eps) ? v.x / pScale.x : v.x,
				(fabs(pScale.y) > eps) ? v.y / pScale.y : v.y,
				(fabs(pScale.z) > eps) ? v.z / pScale.z : v.z
			);
		}
		else
		{
			// 부모가 없으면 로컬 = 월드
			m_localScale = worldScaleBefore;
			m_localRotation = worldRotationBefore;
			m_localPosition = worldPositionBefore;
		}
	}

	MarkDirty();
	onMatrixUpdate.Invoke(this);  //GetGameObject()->UpdateActiveInHierarchy(); // 부모가 바뀌었으므로 Hierarchy 활성화 상태 업데이트
}
