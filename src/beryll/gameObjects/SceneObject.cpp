#include "SceneObject.h"
#include "beryll/renderer/Renderer.h"

namespace Beryll
{
    void SceneObject::addMaterial2(const std::string& diffusePath, const std::string& specularPath,
                                   const std::string& normalMapPath, const std::string& blendTexturePath)
    {
        m_material2 = BeryllUtils::Common::loadMaterial2(diffusePath, specularPath, normalMapPath, blendTexturePath);

        BR_ASSERT(((m_material1.diffTexture != nullptr && m_material2->diffTexture != nullptr)),
                  "%s", "m_material1 and m_material2 both must have diffTexture.");

        BR_ASSERT(((m_material1.specTexture != nullptr && m_material2->specTexture != nullptr) ||
                   (m_material1.specTexture == nullptr && m_material2->specTexture == nullptr)),
                  "%s", "m_material1 and m_material2 both must have specTexture or both dont.");

        BR_ASSERT(((m_material1.normalMapTexture != nullptr && m_material2->normalMapTexture != nullptr) ||
                   (m_material1.normalMapTexture == nullptr && m_material2->normalMapTexture == nullptr)),
                  "%s", "m_material1 and m_material2 both must have normalMapTexture or both dont.");

        BR_ASSERT(((m_material2->blendTexture != nullptr)), "%s", "m_material2 must have blendTexture.");

        // Create different shader for two materials.
        if(m_isAnimatedObject)
        {
            m_internalShader = Renderer::createShader(BeryllConstants::animatedObjTwoMaterialsDefaultVertexPath.data(),
                                                      BeryllConstants::animatedObjTwoMaterialsDefaultFragmentPath.data());
        }
        else
        {
            m_internalShader = Renderer::createShader(BeryllConstants::simpleObjTwoMaterialsDefaultVertexPath.data(),
                                                      BeryllConstants::simpleObjTwoMaterialsDefaultFragmentPath.data());
        }

        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();

        if(m_material1.specTexture)
            m_internalShader->activateSpecularTextureMat1();

        if(m_material1.normalMapTexture)
            m_internalShader->activateNormalMapTextureMat1();

        m_internalShader->activateDiffuseTextureMat2();

        if(m_material2->specTexture)
            m_internalShader->activateSpecularTextureMat2();

        if(m_material2->normalMapTexture)
            m_internalShader->activateNormalMapTextureMat2();

        m_internalShader->activateBlendTextureMat2();

        BR_INFO("%s", "Loaded material 2 and created new shader.");
    }
}
