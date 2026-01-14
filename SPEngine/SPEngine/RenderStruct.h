#pragma once

#include "framework.h"

#define BONE_MAXSIZE 256

enum RenderType {
	SKYBOX = 0,
	PREDEPTH = 1,
	PBR = 2,
	PHONG = 3,
	TRANSCULANT = 4,
	POSTPROCESS = 5,
	UI = 6
};

struct Render_CamBuffer {
	DirectX::SimpleMath::Matrix mView = DirectX::SimpleMath::Matrix::Identity;			// 카메라좌표계 변환행렬
	DirectX::SimpleMath::Matrix mProjection = DirectX::SimpleMath::Matrix::Identity;	// ndc좌표계 변환행렬
	DirectX::SimpleMath::Vector4 camPos;
};

struct Render_TransformBuffer
{
	DirectX::SimpleMath::Matrix mWorld;
	DirectX::SimpleMath::Matrix mNormalMatrix;
};

struct Render_LightBuffer {
	DirectX::SimpleMath::Vector4 vLightDir;
	DirectX::SimpleMath::Vector4 vLightColor;
};

struct Render_MaterialBuffer
{
	DirectX::SimpleMath::Vector4 Matambient;	// 메테리얼 정보
	DirectX::SimpleMath::Vector4 Matdiffuse;
	DirectX::SimpleMath::Vector4 Matspecular;

	int shiness;
	DirectX::SimpleMath::Vector3 padding;
};

struct Render_PBRMaterialBuffer
{
    DirectX::SimpleMath::Vector4 BaseColor;	// 메테리얼 정보
    
    float Metalic = 0.0f;
    float mRoughness = 0.0f;
    float mAoStrength = 1.0f;
    float mEmissive = 1.0f;
};

struct Render_ShadowBuffer
{
	DirectX::SimpleMath::Matrix ShadowView;
	DirectX::SimpleMath::Matrix ShadowProjection;
};