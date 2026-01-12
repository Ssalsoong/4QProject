#pragma once
#include "ScriptBehaviour.h"
#include "Transform.h"
#include "MMMInput.h"
#include "MMMTime.h"
#include "MMMApplication.h"

namespace MMMEngine
{
	class PlayerMove : public ScriptBehaviour
	{
	public:
		PlayerMove()
		{
			SetExecutionOrder(10);
			REGISTER_BEHAVIOUR_MESSAGE(Update);
		}
		float moveSpeed = 145.0f;
		float m_averageFps = 0.0f;
		float m_timeSinceLastUpdate = 0.0f; // 마지막 FPS 갱신 후 경과 시간
		float m_frameTimeAccumulator = 0.0f; // 프레임 시간 누적 합
		int m_frameCount = 0; // 프레임 수
		const float FPS_UPDATE_INTERVAL = 0.5f; // 0.5초마다 평균 FPS를 갱신합니다.

		void Update()
		{
			float hInput = (Input::GetKey(KeyCode::RightArrow) ? 1.0f : 0.0f) + (Input::GetKey(KeyCode::LeftArrow) ? -1.0f : 0.0f);
			float vInput = (Input::GetKey(KeyCode::UpArrow) ? 1.0f : 0.0f) + (Input::GetKey(KeyCode::DownArrow) ? -1.0f : 0.0f);

			Vector2 vec = { hInput, vInput };
			vec.Normalize();

			GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + (vec * moveSpeed * Time::GetDeltaTime()));

			float deltaTime = Time::GetDeltaTime();

			GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + (vec * moveSpeed * deltaTime));

			// --- 2. FPS 평균 계산 로직 ---
			m_timeSinceLastUpdate += deltaTime;
			m_frameTimeAccumulator += deltaTime;
			m_frameCount++;

			// 설정된 갱신 간격(0.5초)이 지나면 평균 FPS를 계산합니다.
			if (m_timeSinceLastUpdate >= FPS_UPDATE_INTERVAL)
			{
				// 평균 프레임 시간 (초당 프레임 수의 역수)을 계산합니다.
				float averageFrameTime = m_frameTimeAccumulator / m_frameCount;

				// 평균 FPS 계산
				m_averageFps = 1.0f / averageFrameTime;

				// 다음 측정을 위해 변수들을 초기화합니다.
				m_timeSinceLastUpdate = 0.0f;
				m_frameTimeAccumulator = 0.0f;
				m_frameCount = 0;
			}

			// --- 3. 창 제목 표시 로직 ---
			// 소수점 2자리까지만 표시하여 깔끔하게 보입니다.
			// (std::wstringstream을 사용하면 소수점 처리가 더 편리합니다.)
			std::wstringstream ss;
			ss << L" player -> pos : { "
				<< std::fixed << std::setprecision(2) << GetTransform()->GetLocalPosition().x
				<< L", "
				<< std::fixed << std::setprecision(2) << GetTransform()->GetLocalPosition().y
				<< L" } Avg FPS : "
				<< std::fixed << std::setprecision(2) << m_averageFps;

			Application::SetWindowTitle(ss.str());
		}
	};
}