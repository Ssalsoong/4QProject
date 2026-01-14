#include "MeshRenderer.h"
#include "StaticMesh.h"
#include "ResourceManager.h"
#include "../BaseEngine/Helper.h"
#include "Renderer.h"
#include "RenderPipe.h"
#include "PBRRenderer.h"
#include "PhongRenderer.h"
#include "GameObject.h"
#include "Transform.h"

using namespace Microsoft::WRL;

void MeshRenderer::SetMesh(std::shared_ptr<StaticMesh>& _mesh)
{
	mesh = _mesh;
	Start();
}

void MeshRenderer::Start()
{
	// 유효성 확인
	if (!mesh || !mesh->meshData || !mesh->gpuBuffer)
		return;
	
	for (auto& [matIdx, meshIndices] : mesh->meshGroupData) {
		auto& material = mesh->meshData->materials[matIdx];

		for (const auto& idx : meshIndices) {
			std::weak_ptr<Renderer> renderer;

			switch (material->matType) {
			case RenderType::PHONG:
				renderer = RenderPipe::GetInstance()->AddRenderer<PhongRenderer>(RenderType::PHONG);
				break;
			//case RenderType::PBR:
				//renderer = RenderPipe::GetInstance()->AddRenderer<PBRRenderer>(RenderType::PBR);
				//break;
			default:
				throw std::runtime_error("MeshRenderer::Unknown Renderer Type!!");
			}

			auto& meshBuffer = mesh->gpuBuffer->vertexBuffers[idx];
			auto& indicesBuffer = mesh->gpuBuffer->indexBuffers[idx];
			UINT indicesSize = static_cast<UINT>(mesh->meshData->indices[idx].size());

			if (auto locked = renderer.lock()) {
				locked->SetRenderData(meshBuffer, indicesBuffer, indicesSize, material);
				renderers.push_back(renderer);
			}
		}
	}

	// TODO:: 렌더파이프에 렌더러 등록하기, 메테리얼 읽어서 렌더러에 메시 정보 보내기.
	//for (int i = 0; i < mesh->meshGroupData.size(); i++) {
	//	// 렌더러 추가 (임시)
	//	auto& material = mesh->meshData->materials[i];

	//	for (const auto& idx : mesh->meshGroupData[i]) {
	//		// TODO :: 렌더러 타입별 추가하는 방식 다르게 하는방법 강구하기
	//		auto renderer = RenderPipe::GetInstance()->AddRenderer<PhongRenderer>(RenderType::PHONG);
	//		auto& meshBuffer = mesh->gpuBuffer->vertexBuffers[idx];
	//		auto& indicesBuffer = mesh->gpuBuffer->indexBuffers[idx];
	//		UINT indicesSize = static_cast<UINT>(mesh->meshData->indices[idx].size());

	//		if (auto locked = renderer.lock()) {
	//			locked->SetRenderData(meshBuffer, indicesBuffer, indicesSize, material);
	//			renderers.push_back(renderer);
	//		}
	//		else
	//			throw std::runtime_error("MeshRenderer : 렌더러가 존재하지 않습니다!!");
	//	}
	//}

	/*if (auto renderer = dynamic_pointer_cast<PBRRenderer>(mesh->modelMaterials[0]->renderer))
		RenderPipe::GetInstance()->AddRenderer<PBRRenderer>(1, renderer);
	else if (auto renderer = dynamic_pointer_cast<PhongRenderer>(mesh->modelMaterials[0]->renderer))
		RenderPipe::GetInstance()->AddRenderer<PhongRenderer>(2, renderer);
	else
		throw std::runtime_error("MeshRenderer::Unknown Renderer Type!!");*/
}

void MeshRenderer::Update()
{
	// 월드 매트릭스 전달
	for (auto& renderer : renderers) {
		if (auto locked = renderer.lock()) {
			if (auto transform = gameObject->transform.lock())
				locked->SetWorldMat(transform->GetMatrix());
		}
	}
}

MeshRenderer::~MeshRenderer()
{
	// 렌더러 제거 명령
	for (auto& renderer : renderers) {
		if (auto locked = renderer.lock()) {
			RenderPipe::GetInstance()->RemoveRenderer(RenderType::PHONG, locked);
		}
	}
}
