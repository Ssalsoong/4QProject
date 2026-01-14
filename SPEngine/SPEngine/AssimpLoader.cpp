#include "AssimpLoader.h"
#include "ResourceManager.h"
#include <filesystem>
#include "StaticMesh.h"
#include "Material.h"
#include "directxtk/SimpleMath.h"
#include "../BaseEngine/Helper.h"
#include "directxtk/WICTextureLoader.h"
#include "Texture2D.h"
#include "PhongMaterial.h"
#include "PhongRenderer.h"
#include "PBRMaterial.h"
#include "PBRRenderer.h"
#include "RenderStruct.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

AssimpLoader* AssimpLoader::instance = nullptr;

bool AssimpLoader::LoadVertex(std::vector<Mesh_Vertex>* _vertices, const aiMesh* _mesh)
{
	for (UINT i = 0; i < _mesh->mNumVertices; i++) {
		Mesh_Vertex v{};

		v.Pos = Vector3(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);

		if (_mesh->HasNormals()) {
			v.Normal = Vector3(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
		}

		if (_mesh->HasTangentsAndBitangents()) {
			v.Tangent = Vector3(_mesh->mTangents[i].x, _mesh->mTangents[i].y, _mesh->mTangents[i].z);
			v.BiTangent = Vector3(_mesh->mBitangents[i].x, _mesh->mBitangents[i].y, _mesh->mBitangents[i].z);
		}

		if (_mesh->HasTextureCoords(0)) {
			v.Tex = Vector2(_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y);
		}

		_vertices->push_back(v);
	}

	return true;
}

bool AssimpLoader::LoadIndex(std::vector<UINT>* _indices, const aiMesh* _mesh)
{
	for (UINT i = 0; i < _mesh->mNumFaces; i++) {
		for (UINT j = 0; j < _mesh->mFaces[i].mNumIndices; j++)
			_indices->push_back(UINT(_mesh->mFaces[i].mIndices[j]));
	}

	return true;
}

bool AssimpLoader::LoadMaterials(std::vector<std::shared_ptr<Material>>& _out, const aiScene* _scene, const std::wstring& _path)
{
	if (!_scene->HasMaterials())
		return false;

	_out.resize(_scene->mNumMaterials);

	for (UINT i = 0; i < _scene->mNumMaterials; i++) {
		aiString aiStr;
		aiMaterial* aiMat = _scene->mMaterials[i];
		auto m_pDevice = ResourceManager::GetInstance()->GetDevice();

		// 현재 기본값은 퐁 메테리얼이다.
		auto currMat = std::make_shared<PhongMaterial>();
		currMat->matType = RenderType::PHONG;
		currMat->renderer = std::make_shared<PhongRenderer>();

		_out[i] = currMat;

		// 베이스 컬러
		aiColor4D baseColor;
		if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == AI_SUCCESS) {
			_out[i]->baseColor = { baseColor.r, baseColor.g, baseColor.b, baseColor.a };
		}
		else if (aiMat->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS) {
			_out[i]->baseColor = { baseColor.r, baseColor.g, baseColor.b, baseColor.a };
		}

		// 디퓨즈
		if (aiMat->GetTextureCount(aiTextureType_DIFFUSE)) {
			if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _path / p.filename();

				// 리소스 매니저에서 불러오기
				auto diffMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(relativePath.wstring());
				if(!diffMat->srv)
					HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, diffMat->srv.GetAddressOf()));

				currMat->diffuse = diffMat;
			}
		}

		// 디퓨즈가 없으면?
		if (!currMat->diffuse) {
			auto diffMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(defaultDiffuse);
			if(!diffMat->srv)
				HR_T(CreateWICTextureFromFile(m_pDevice, defaultDiffuse.c_str(), nullptr, diffMat->srv.GetAddressOf()));

			currMat->diffuse = diffMat;
		}

		// 스페큘러
		if (aiMat->GetTextureCount(aiTextureType_SPECULAR)) {
			if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _path / p.filename();

				auto specMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(relativePath.wstring());
				if (!specMat->srv)
					HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, specMat->srv.GetAddressOf()));

				currMat->specular = specMat;
			}
		}

		// 노말
		if (aiMat->GetTextureCount(aiTextureType_NORMALS)) {
			if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _path / p.filename();

				auto normalMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(relativePath.wstring());
				if (!normalMat->srv)
					HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, normalMat->srv.GetAddressOf()));

				currMat->normal = normalMat;

				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].normal));
			}
		}

		// 이미션
		if (aiMat->GetTextureCount(aiTextureType_EMISSIVE)) {
			if (aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _path / p.filename();

				auto emissMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(relativePath.wstring());
				if (!emissMat->srv)
					HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, emissMat->srv.GetAddressOf()));

				currMat->emissive = emissMat;

				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].emissive));
			}
		}
	}

	return true;
}

bool AssimpLoader::LoadMeshBuffers(std::shared_ptr<MeshGPU>& _out, std::shared_ptr<MeshData>& _meshData)
{
	// shared 생성
	_out = std::make_shared<MeshGPU>();

	// 디바이스 가져오기
	const auto device = ResourceManager::GetInstance()->GetDevice();

	// 버텍스 버퍼 생성
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	for (int i = 0; i < _meshData->vertices.size(); i++) {
		bd.ByteWidth = UINT(sizeof(Mesh_Vertex) * _meshData->vertices[i].size());
		vbData.pSysMem = _meshData->vertices[i].data();

		Microsoft::WRL::ComPtr<ID3D11Buffer> tempBuffer = nullptr;
		HR_T(device->CreateBuffer(&bd, &vbData, &tempBuffer));

		if (tempBuffer)
			_out->vertexBuffers.push_back(tempBuffer);
	}

	// 인덱스 버퍼 생성
	bd = {};
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};


	for (int i = 0; i < _meshData->indices.size(); i++) {
		bd.ByteWidth = UINT(sizeof(UINT) * _meshData->indices[i].size());
		ibData.pSysMem = _meshData->indices[i].data();

		Microsoft::WRL::ComPtr<ID3D11Buffer> tempBuffer = nullptr;
		HR_T(device->CreateBuffer(&bd, &ibData, &tempBuffer));

		if (tempBuffer)
			_out->indexBuffers.push_back(tempBuffer);
	}

	return true;
}

AssimpLoader* AssimpLoader::GetInstance()
{
	if (instance == nullptr) {
		instance = new AssimpLoader();
	}
	return instance;
}

void AssimpLoader::Initialize()
{
	
}

AssimpLoader::~AssimpLoader()
{
	
}

std::shared_ptr<StaticMesh> AssimpLoader::LoadStaticMesh(std::wstring _filePath)
{
	UINT importFlags =
		aiProcess_CalcTangentSpace |
		aiProcess_GenUVCoords |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_GenNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_ConvertToLeftHanded;

	// 경로 가공
	std::filesystem::path p = _filePath.c_str();

	// 유효성 확인
	if (!p.has_filename())
		throw std::exception("AssimpLoader::LoadStaticMesh : path is not file!!");
	if (!std::filesystem::exists(p))
		throw std::exception("AssimpLoader::LoadStaticMesh : file is not found!!");

	// 리소스 로드
	auto scene = m_importer.ReadFile(p.string().c_str(), importFlags);
	auto meshResource = ResourceManager::GetInstance()->LoadFile<StaticMesh>(p.wstring());

	// 임포트 성공여부 확인
	if (!scene || !scene->HasMeshes()) {
		std::string assErrStr = m_importer.GetErrorString();
		std::string errorStr = "AssimpLoader::LoadStaticMesh : Scene load Error!! ("
			+ assErrStr + ")";
		throw std::exception(errorStr.c_str());
	}

	if (!meshResource->meshData) {
		auto meshData = std::make_shared<MeshData>();
		
		// 메시 그룹 <MatIdx, vec<MeshIdx>>
		std::unordered_map<UINT, std::vector<UINT>> meshGroupData;

		// 메시 로딩
		for (UINT i = 0; i < scene->mNumMeshes; i++) {
			std::vector<Mesh_Vertex> tempV;
			std::vector<UINT> tempI;
			LoadVertex(&tempV, scene->mMeshes[i]);
			LoadIndex(&tempI, scene->mMeshes[i]);

			meshData->vertices.push_back(tempV);
			meshData->indices.push_back(tempI);

			meshGroupData[scene->mMeshes[i]->mMaterialIndex].push_back(i);
		}

		// 메시그룹 저장
		meshResource->meshGroupData = meshGroupData;

		// 메테리얼 로딩
		LoadMaterials(meshData->materials, scene, p.parent_path().wstring());

		// 메시데이터 리소스 등록
		meshResource->meshData = meshData;

		// MeshBuffer 생성
		LoadMeshBuffers(meshResource->gpuBuffer, meshResource->meshData);
	}

	return meshResource;
};

void AssimpLoader::LoadSkeletalMesh(std::wstring _filePath)
{

}

//void AssimpLoader::LoadMesh(std::wstring _filePath)
//{
//
//}
