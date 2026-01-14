#pragma once
#include <unordered_map>
#include "Component.h"
#include "framework.h"

class StaticMesh;
class Renderer;
class MeshRenderer : public Component
{
public:
	// GPU ¹öÆÛ
	std::shared_ptr<StaticMesh> mesh = nullptr;
	std::vector<std::weak_ptr<Renderer>> renderers;

	void SetMesh(std::shared_ptr<StaticMesh>& _mesh);
	void Start() override;
	void Update() override;

	~MeshRenderer();
};

