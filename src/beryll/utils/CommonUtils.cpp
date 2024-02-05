#include "CommonUtils.h"

#include "beryll/renderer/Renderer.h"

namespace BeryllUtils
{
    int Common::m_id = 0;

    Beryll::Material1 Common::loadMaterial1(aiMaterial* material, const std::string& filePath)
    {
        BR_ASSERT((filePath.find_last_of('/') != std::string::npos), "Texture + model must be in folder: %s", filePath.c_str());

        Beryll::Material1 mat1;
        std::string texturePath;

        if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString textName;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &textName);

            std::string textName2 = textName.C_Str();
            for(int g = static_cast<int>(textName2.size()) - 1; g >= 0; --g)
            {
                if(textName2[g] == '/' || textName2[g] == '\\')
                {
                    textName2 = textName2.substr(g + 1);
                    break;
                }
            }
            texturePath = filePath.substr(0, filePath.find_last_of('/'));
            texturePath += '/';
            texturePath += textName2;
            BR_INFO("Diffuse texture here: %s", texturePath.c_str());

            mat1.diffTexture = Beryll::Renderer::createTexture(texturePath.c_str(), Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        }

        if(material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            aiString textName;
            material->GetTexture(aiTextureType_SPECULAR, 0, &textName);

            std::string textName2 = textName.C_Str();
            for(int g = static_cast<int>(textName2.size()) - 1; g >= 0; --g)
            {
                if(textName2[g] == '/' || textName2[g] == '\\')
                {
                    textName2 = textName2.substr(g + 1);
                    break;
                }
            }
            texturePath = filePath.substr(0, filePath.find_last_of('/'));
            texturePath += '/';
            texturePath += textName2;
            BR_INFO("Specular texture here: %s", texturePath.c_str());

            mat1.specTexture = Beryll::Renderer::createTexture(texturePath.c_str(), Beryll::TextureType::SPECULAR_TEXTURE_MAT_1);
        }

        if(material->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            aiString textName;
            material->GetTexture(aiTextureType_NORMALS, 0, &textName);

            std::string textName2 = textName.C_Str();
            for(int g = static_cast<int>(textName2.size()) - 1; g >= 0; --g)
            {
                if(textName2[g] == '/' || textName2[g] == '\\')
                {
                    textName2 = textName2.substr(g + 1);
                    break;
                }
            }
            texturePath = filePath.substr(0, filePath.find_last_of('/'));
            texturePath += '/';
            texturePath += textName2;
            BR_INFO("Normal map texture here: %s", texturePath.c_str());

            mat1.normalMapTexture = Beryll::Renderer::createTexture(texturePath.c_str(), Beryll::TextureType::NORMAL_MAP_TEXTURE_MAT_1);
        }

        return mat1;
    }
}
