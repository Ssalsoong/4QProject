#pragma once
#include <memory>
#include "GameResource.h"
#include "Texture2D.h"
#include "Renderer.h"
#include "RenderStruct.h"

class Material : public GameResource
{
public:
	virtual ~Material() = default;
	RenderType matType = RenderType::PHONG;
	std::shared_ptr<Renderer> renderer = nullptr;

	DirectX::SimpleMath::Color baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };	// ∫£¿ÃΩ∫ ƒ√∑Ø
	//std::shared_ptr<Texture2D> diffuse = nullptr;		// µ«ª¡Ó∏ 
	//std::shared_ptr<Texture2D> specular = nullptr;		// Ω∫∆Â≈ß∑Ø∏ 
	//std::shared_ptr<Texture2D> normal = nullptr;		// ≥Î∏÷∏ 
	//std::shared_ptr<Texture2D> emissive = nullptr;		// πﬂ±§∏ 
};

