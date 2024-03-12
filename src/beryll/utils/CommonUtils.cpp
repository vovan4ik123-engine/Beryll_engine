#include "CommonUtils.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/utils/File.h"
#include "beryll/utils/Matrix.h"

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
        else
        {
            BR_ASSERT(false, "%s", "Material1 must have at least one diffuse texture.");
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

    std::optional<Beryll::Material2> Common::loadMaterial2(const std::string& diffusePath, const std::string& specularPath,
                                                           const std::string& normalMapPath, const std::string& blendTexturePath)
    {
        BR_ASSERT((!diffusePath.empty() && !blendTexturePath.empty()), "%s", "Material2 must have diffuse texture and blend texture.");

        Beryll::Material2 mat2;
        mat2.diffTexture = Beryll::Renderer::createTexture(diffusePath.c_str(), Beryll::TextureType::DIFFUSE_TEXTURE_MAT_2);
        mat2.blendTexture = Beryll::Renderer::createTexture(blendTexturePath.c_str(), Beryll::TextureType::BLEND_TEXTURE_MAT_2);

        if(!specularPath.empty())
            mat2.specTexture = Beryll::Renderer::createTexture(specularPath.c_str(), Beryll::TextureType::SPECULAR_TEXTURE_MAT_2);

        if(!normalMapPath.empty())
            mat2.normalMapTexture = Beryll::Renderer::createTexture(normalMapPath.c_str(), Beryll::TextureType::NORMAL_MAP_TEXTURE_MAT_2);

        return mat2;
    }

    std::vector<glm::vec3> Common::loadMeshVerticesToVector(const char* filePath)
    {
        BR_ASSERT((std::string(filePath).substr(std::string(filePath).find_last_of('.')) == ".fbx"),
                  "Mesh must be in .fbx file: %s", filePath);

        BR_INFO("Loading vertices to vector from file: %s", filePath);

        uint32_t bufferSize = 0;
        char* buffer = BeryllUtils::File::readToBuffer(filePath, &bufferSize);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(buffer, bufferSize, 0);
        delete[] buffer;
        if(!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
        {
            BR_ASSERT(false, "Scene loading error for file: %s", filePath);
        }

        BR_ASSERT((scene->mNumMeshes == 1), "Mesh with vertices: %s MUST contain only 1 mesh.", filePath);

        const aiMesh* mesh = scene->mMeshes[0];
        glm::mat4 modelMatrix{1.0f};
        const aiNode* node = BeryllUtils::Common::findAinodeForAimesh(scene, scene->mRootNode, mesh->mName);
        if(node)
            modelMatrix = BeryllUtils::Matrix::aiToGlm(node->mTransformation);


        std::vector<glm::vec3> vertices;
        vertices.reserve(mesh->mNumVertices);

        glm::vec4 vertex4{0.0f};
        for(int i = 0; i < mesh->mNumVertices; ++i)
        {
            vertex4.x = mesh->mVertices[i].x;
            vertex4.y = mesh->mVertices[i].y;
            vertex4.z = mesh->mVertices[i].z;
            vertex4.w = 1.0f;

            vertex4 = modelMatrix * vertex4;

            glm::vec3 vertex3{vertex4};
            if(std::find(vertices.begin(), vertices.end(), vertex3) == vertices.end())
            {
                vertices.push_back(vertex3);
            }
        }

        return vertices;
    }
}
