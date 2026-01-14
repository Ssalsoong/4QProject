#pragma once
#include "framework.h"
#include "Material.h"

struct Mesh_Vertex
{
	DirectX::SimpleMath::Vector3 Pos;		// 정점 위치 정보
	DirectX::SimpleMath::Vector3 Normal;	// 노멀
	DirectX::SimpleMath::Vector3 Tangent;	// 탄젠트
	DirectX::SimpleMath::Vector3 BiTangent;	// Bi탄젠트
	DirectX::SimpleMath::Vector2 Tex;		// 텍스쳐 UV
	int boneIndices[4] = { -1, -1, -1, -1 };				// 버텍스와 연결된 본들의 인덱스
	float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };		// 각 본들의 가중치
};

struct Mesh_BoneBuffer
{
	DirectX::SimpleMath::Matrix boneMat[BONE_MAXSIZE] = { DirectX::SimpleMath::Matrix::Identity, };
};

struct MeshData {
	std::vector<std::vector<Mesh_Vertex>> vertices;
	std::vector<std::vector<UINT>> indices;
	std::vector<std::shared_ptr<Material>> materials;
};

struct MeshGPU {
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertexBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> indexBuffers;
};
