#pragma once
#include "Component.h"
#include "framework.h"

class DirectionalLight : public Component {
	// Á¤º¸
	DirectX::XMFLOAT4 m_LightColors = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 m_LightDirs = DirectX::XMFLOAT4(0.0f, -1.0f, 1.0f, 1.0f);
};

