#pragma once
#include "Material.h"
class PBRMaterial : public Material
{
public:
    std::shared_ptr<Texture2D> albedo;
    std::shared_ptr<Texture2D> normal;
    std::shared_ptr<Texture2D> metallic;
    std::shared_ptr<Texture2D> roughness;
    std::shared_ptr<Texture2D> ao;
    std::shared_ptr<Texture2D> emissive;

    Render_PBRMaterialBuffer matBuffer;
};

