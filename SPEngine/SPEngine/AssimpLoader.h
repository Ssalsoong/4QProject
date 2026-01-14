#pragma once
#include "framework.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "RenderStruct.h"
#include "StaticMesh.h"

class Material;
class AssimpLoader
{
private:
	AssimpLoader() = default;
	~AssimpLoader();

	static AssimpLoader* instance;
	Assimp::Importer m_importer;

	bool LoadVertex(std::vector<Mesh_Vertex>* _vertices, const aiMesh* _mesh);
	bool LoadIndex(std::vector<UINT>* _indices, const aiMesh* _mesh);
	bool LoadMaterials(std::vector<std::shared_ptr<Material>>& _out, const aiScene* _scene, const std::wstring& _path);
	bool LoadMeshBuffers(std::shared_ptr<MeshGPU>& _out, std::shared_ptr<MeshData>& _meshData);

	std::wstring defaultDiffuse = L"../Resources/Texture/Default/Solid_White.png";
public:
	static AssimpLoader* GetInstance();

	void Initialize();

	std::shared_ptr<StaticMesh> LoadStaticMesh(std::wstring _filePath);
	void LoadSkeletalMesh(std::wstring _filePath);

	//void LoadMesh(std::wstring _filePath);
};

