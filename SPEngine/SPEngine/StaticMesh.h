#pragma once
#include "framework.h"
#include <assimp/scene.h>
#include "GameResource.h"
#include "ResourceShared.h"

class StaticMesh : public GameResource
{
public:
	// 메시 데이터
	std::shared_ptr<MeshData> meshData;		
	// GPU 버퍼
	std::shared_ptr<MeshGPU> gpuBuffer;		
	// 메시 그룹 <MatIdx, MeshIdx>
	std::unordered_map<UINT, std::vector<UINT>> meshGroupData;

	bool castShadows = true;
	bool receiveShadows = true;
};

